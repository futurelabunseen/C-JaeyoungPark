#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "Templates/SharedPointer.h"

// 포인터로만 사용하므로, 전체 정의를 포함할 필요 없이 전방 선언
class FGameOctree;
class FEvent;
class AActor; // ◀ AActor 전방 선언 추가

// 게임 스레드와 독립적으로 옥트리 재구축 연산을 수행하는 워커 스레드 클래스
class FOctreeBuilderWorker : public FRunnable
{
public:

    // InBounds : 옥트리가 커버할 전체 월드의 경계
    // InMaxElements : 분할되기 전까지 하나의 노드가 가질 수 있는 최대 요소 수
    FOctreeBuilderWorker(const FBox& InBounds, int32 InMaxElements);
    virtual ~FOctreeBuilderWorker();

    // FRunnable 인터페이스를 반드시 구현해야 하는 함수들
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    virtual void Exit() override;

    // 게임 스레드에서 호출하여 워커에게 새 작업을 시작하라고 알리는 함수
    // ActorsSnapshot : 재구축할 옥트리에 삽입될 액터들의 목록
    void StartWork(TArray<AActor*> ActorsSnapshot);

    // 워커가 작업을 완료했는지 여부를 나타내는 스레드 안전 플래그
    FThreadSafeBool bIsWorkDone;

    // 워커가 완성한 옥트리의 소유권을 게임 스레드로 이전하는 함수
    // 완성된 옥트리를 가리키는 TUniquePtr
    TUniquePtr<FGameOctree> GetResult();

private:
    // 스레드를 안전하게 중지시키기 위한 스레드 안전 플래그
    FThreadSafeBool bShouldStop;

    // 스레드의 작업 시작/대기를 제어하는 동기화 객체
    TUniquePtr<FEvent> WorkEvent;

    // 생성자에서 전달받은 옥트리 생성 정보
    FBox Bounds;
    int32 MaxElements;

    // 게임 스레드로부터 복사해 온, 처리해야 할 액터 목록
    TArray<AActor*> ActorsToProcess;

    // ◀◀◀ 이전 옥트리(PendingOctree)를 참조하기 위한 SharedPtr 멤버 변수 추가
    //TSharedPtr<FGameOctree, ESPMode::ThreadSafe> BaseOctreeForBuild;

    // 워커 스레드 내부에서 만들고 완성할 옥트리
    TUniquePtr<FGameOctree> BuiltOctree;
};