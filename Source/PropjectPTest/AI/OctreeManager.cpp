#include "AI/OctreeManager.h"
#include "AI/Octree.h"
#include "AI/OctreeBuilderWorker.h"
#include "HAL/RunnableThread.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

AOctreeManager::AOctreeManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AOctreeManager::BeginPlay()
{
    Super::BeginPlay();

    // 워커(일꾼)의 명세서(FOctreeBuilderWorker)를 만듦
    ActiveOctree = MakeShared<FGameOctree, ESPMode::ThreadSafe>(WorldBounds, MaxElementsPerNode);
    PendingOctree = MakeShared<FGameOctree, ESPMode::ThreadSafe>(WorldBounds, MaxElementsPerNode);

    Worker = new FOctreeBuilderWorker(WorldBounds, MaxElementsPerNode);

    // 실제 스레드(FRunnableThread)를 생성하고, 워커를 할당하여 실행시킴
    WorkerThread = FRunnableThread::Create(Worker, TEXT("OctreeBuilderThread"), 0, TPri_Normal);
}

void AOctreeManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // --- 가장 안전하고 표준적인 수동 스레드 종료 절차 ---

    if (WorkerThread)
    {
        // 1. 스레드에게 종료를 요청
        if (Worker)
        {
            Worker->Stop();
        }

        // 2. 스레드가 실제로 종료될 때까지 기다림
        WorkerThread->WaitForCompletion();

        // 3. 스레드 관리자 객체를 삭제
        delete WorkerThread;
        WorkerThread = nullptr;
    }

    // 4. 스레드가 완전히 멈춘 것을 확인한 뒤, FRunnable 객체를 안전하게 삭제
    if (Worker)
    {
        delete Worker;
        Worker = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

void AOctreeManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 워커가 작업을 마쳤는지(bIsWorkDone) 확인
    if (Worker && Worker->bIsWorkDone)
    {
        // GetResult()로 완성된 옥트리의 소유권을 가져옴
        TUniquePtr<FGameOctree> NewOctreeResult = Worker->GetResult();
        if (NewOctreeResult)
        {
            // TUniquePtr의 소유권을 TSharedPtr로 안전하게 이전
            TSharedPtr<FGameOctree, ESPMode::ThreadSafe> NewPendingTree(NewOctreeResult.Release());

            // "더블 버퍼링" 기법을 사용하여 옥트리를 안전하게 교체
            // 다른 스레드가 옥트리를 사용하고 있을 수 있으므로, 교체하는 순간을 잠금
            FScopeLock Lock(&OctreeSwapSection);
            TSharedPtr<FGameOctree, ESPMode::ThreadSafe> TempTree = ActiveOctree;
            ActiveOctree = NewPendingTree;
            PendingOctree = TempTree;
        }
    }

    TimeSinceLastUpdate += DeltaTime;
    // 작업할 준비가 되었고(bIsWorkDone), 업데이트 주기(UpdateInterval)가 되었는지 확인
    if (Worker && Worker->bIsWorkDone && TimeSinceLastUpdate >= UpdateInterval)
    {
        TimeSinceLastUpdate = 0.0f;

        // 다른 곳에서 액터 목록을 수정하지 못하도록 잠그고(FScopeLock) 안전하게 복사
        TArray<AActor*> ActorsSnapshot;
        {
            FScopeLock Lock(&RegisteredActorsSection);
            ActorsSnapshot = RegisteredActors;
        }

        // 워커에게 작업을 지시
        //Worker->StartWork(MoveTemp(ActorsSnapshot));
    }

    if (bDrawDebug && ActiveOctree.IsValid())
    {
        ActiveOctree->DrawDebug(GetWorld());
    }
}

void AOctreeManager::RegisterActor(AActor* Actor)
{
    if (IsValid(Actor))
    {
        FScopeLock Lock(&RegisteredActorsSection);
        RegisteredActors.AddUnique(Actor);
    }
}

void AOctreeManager::UnregisterActor(AActor* Actor)
{
    FScopeLock Lock(&RegisteredActorsSection);
    RegisteredActors.Remove(Actor);
}

void AOctreeManager::FindNearbyActors(const FVector& Location, float Radius, TArray<AActor*>& OutActors)
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