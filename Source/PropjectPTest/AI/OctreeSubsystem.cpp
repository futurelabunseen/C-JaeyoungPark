#include "AI/OctreeSubsystem.h"
#include "HAL/RunnableThread.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

void UOctreeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // 틱 델리게이트를 등록합니다.
    TickDelegateHandle = FWorldDelegates::OnWorldTickStart.AddUObject(this, &UOctreeSubsystem::CustomTick);
}

void UOctreeSubsystem::Deinitialize()
{

    // 1. 스레드를 안전하게 종료합니다.
    if (WorkerThread)
    {
        if (Worker.IsValid())
        {
            Worker->Stop();
        }
        WorkerThread->WaitForCompletion();
        delete WorkerThread;
        WorkerThread = nullptr;
    }
    Worker.Reset();

    // 2. 틱 델리게이트를 해제합니다.
    FWorldDelegates::OnWorldTickStart.Remove(TickDelegateHandle);
    ActiveOctree.Reset(); // ◀ ActiveOctree도 여기서 리셋

    Super::Deinitialize();
}

void UOctreeSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    // 월드가 시작될 때 옥트리와 스레드를 생성합니다.
    ActiveOctree = MakeShared<FGameOctree, ESPMode::ThreadSafe>(WorldBounds, MaxElementsPerNode);
    Worker = MakeShared<FOctreeBuilderWorker>(WorldBounds, MaxElementsPerNode);
    WorkerThread = FRunnableThread::Create(Worker.Get(), TEXT("OctreeBuilderThread"), 0, TPri_Normal);
}

void UOctreeSubsystem::CustomTick(UWorld* World, ELevelTick TickType, float DeltaTime)
{
    // 이 Subsystem이 속한 월드의 틱이 아닐 경우 무시
    if (World != GetWorld()) return;

    // 게임플레이 중에만 실행
    if (TickType == ELevelTick::LEVELTICK_All && GetWorld()->IsGameWorld())
    {
        if (Worker.IsValid() && Worker->bIsWorkDone)
        {
            TUniquePtr<FGameOctree> NewOctreeResult = Worker->GetResult();
            if (NewOctreeResult)
            {
                TSharedPtr<FGameOctree, ESPMode::ThreadSafe> NewActiveTree(NewOctreeResult.Release());
                FScopeLock Lock(&OctreeSwapSection);
                ActiveOctree = NewActiveTree;
            }
        }

        TimeSinceLastUpdate += DeltaTime;
        if (Worker.IsValid() && Worker->bIsWorkDone && TimeSinceLastUpdate >= UpdateInterval)
        {
            TimeSinceLastUpdate = 0.0f;
            TArray<AActor*> ActorsSnapshot;
            {
                FScopeLock Lock(&RegisteredActorsSection);
                RegisteredActors.RemoveAll([](const TWeakObjectPtr<AActor>& Ptr) { return !Ptr.IsValid(); });
                for (const TWeakObjectPtr<AActor>& Ptr : RegisteredActors)
                {
                    if (Ptr.IsValid())
                    {
                        AActor* Actor = Ptr.Get();
                        if (Actor->ActorHasTag(FName("Player")) || Actor->ActorHasTag(FName("Monster"))) ActorsSnapshot.Add(Actor);
                    }
                }
            }
            if (ActorsSnapshot.Num() > 0) Worker->StartWork(MoveTemp(ActorsSnapshot));
        }
        if (bDrawDebug && ActiveOctree.IsValid() && GetWorld()) ActiveOctree->DrawDebug(GetWorld());
    }
}

void UOctreeSubsystem::RegisterActor(AActor* Actor)
{
    if (IsValid(Actor))
    {
        FScopeLock Lock(&RegisteredActorsSection);
        RegisteredActors.AddUnique(Actor);
    }
}

// UnregisterActor 람다 캡처 방식 수정
void UOctreeSubsystem::UnregisterActor(AActor* Actor)
{
    FScopeLock Lock(&RegisteredActorsSection);
    // TWeakObjectPtr의 operator==를 활용하여 안전하게 비교 및 제거
    RegisteredActors.RemoveSingle(Actor); // TWeakObjectPtr<AActor>는 AActor*와 직접 비교 가능합니다.
}

void UOctreeSubsystem::FindNearbyActors(const FVector& Location, float Radius, TArray<AActor*>& OutActors)
{
    TSharedPtr<FGameOctree, ESPMode::ThreadSafe> OctreeToQuery;
    {
        FScopeLock Lock(&OctreeSwapSection);
        OctreeToQuery = ActiveOctree;
    }

    if (OctreeToQuery.IsValid())
    {
        OctreeToQuery->Query(FSphere(Location, Radius), OutActors);
    }
}