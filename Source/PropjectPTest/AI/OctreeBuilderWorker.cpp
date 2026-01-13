#include "AI/OctreeBuilderWorker.h"
#include "HAL/Event.h"
#include "AI/Octree.h"
#include "HAL/PlatformProcess.h"     // FPlatformProcess::CreateSynchEvent를 사용하기 위해 꼭 필요
#include "GameFramework/Actor.h" // IsValid(Actor)를 위해 필요

FOctreeBuilderWorker::FOctreeBuilderWorker(const FBox& InBounds, int32 InMaxElements)
    : Bounds(InBounds), MaxElements(InMaxElements)
{
    bIsWorkDone = true; // 처음에는 할 일이 없는 상태
    bShouldStop = false;
    WorkEvent.Reset(FPlatformProcess::CreateSynchEvent());
}

FOctreeBuilderWorker::~FOctreeBuilderWorker()
{
    // WorkEvent는 TUniquePtr로 관리되므로 Reset() 시 자동으로 FreeSynchEvent 호출
    // WorkEvent = nullptr; (TUniquePtr는 Reset()만으로 충분)
}

bool FOctreeBuilderWorker::Init()
{
    // 스레드 시작 시 필요한 초기화 작업이 있다면 여기에 작성
    // 지금은 특별한 작업이 없으므로 true를 반환하여 시작을 알림
    return true;
}

uint32 FOctreeBuilderWorker::Run()
{
    while (!bShouldStop)
    {
        if (WorkEvent && WorkEvent->Wait())
        {
            if (bShouldStop) break;

            // 항상 새 옥트리를 처음부터 생성합니다.
            BuiltOctree = MakeUnique<FGameOctree>(Bounds, MaxElements);

            // 게임 스레드로부터 전달받은 (최신) 액터 목록을 순회하며 옥트리에 삽입
            if (BuiltOctree.IsValid())
            {
                for (AActor* Actor : ActorsToProcess)
                {
                    if (IsValid(Actor)) BuiltOctree->Insert(FGameOctreeElement(Actor));
                }
            }
            bIsWorkDone = true;
        }
    }
    return 0;
}

void FOctreeBuilderWorker::Stop()
{
    // 스레드를 종료하라는 신호를 보냄
    bShouldStop = true;

    // 스레드가 WorkEvent->Wait() 상태에서 무한정 대기하는 것을 막기 위해 이벤트를 강제로 발생시켜 Run() 루프를 깨움
    if (WorkEvent)
    {
        WorkEvent->Trigger();
    }
}

void FOctreeBuilderWorker::Exit()
{
}

// StartWork 함수의 시그니처에 BaseOctree 인자를 추가했습니다.
void FOctreeBuilderWorker::StartWork(TArray<AActor*> ActorsSnapshot)
{
    if (bIsWorkDone)
    {
        bIsWorkDone = false; // "작업 중" 상태로 전환
        ActorsToProcess = MoveTemp(ActorsSnapshot); // 스냅샷 데이터의 소유권을 이전받음

        // 전달받은 BaseOctree를 멤버 변수에 저장합니다.
        // BaseOctreeForBuild = BaseOctree;

        if (WorkEvent)
        {
            WorkEvent->Trigger();
        }
    }
}

TUniquePtr<FGameOctree> FOctreeBuilderWorker::GetResult()
{
    // 완성된 옥트리의 소유권을 이 함수를 호출한 쪽(게임 스레드)으로 넘김
    return MoveTemp(BuiltOctree);
}