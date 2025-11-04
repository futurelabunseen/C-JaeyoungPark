#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OctreeManager.generated.h"

// 필요한 클래스들을 미리 알려줌 (전방 선언)
class FRunnableThread;
class FOctreeBuilderWorker;
class FGameOctree;

UCLASS()
class PROPJECTPTEST_API AOctreeManager : public AActor
{
    GENERATED_BODY()

public:
    AOctreeManager();

    // 외부에서 호출할 함수들
    void RegisterActor(AActor* Actor);
    void UnregisterActor(AActor* Actor);
    void FindNearbyActors(const FVector& Location, float Radius, TArray<AActor*>& OutActors);

protected:
    // Actor 라이프사이클 함수
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // 스레드 안전 종료를 위해 필수

private:
    // --- 멀티스레딩을 위한 멤버 변수들 ---

    // 1. 더블 버퍼링을 위한 두 개의 옥트리
    TSharedPtr<FGameOctree, ESPMode::ThreadSafe> ActiveOctree;
    TSharedPtr<FGameOctree, ESPMode::ThreadSafe> PendingOctree;

    // 2. 워커 스레드 객체
    FRunnableThread* WorkerThread = nullptr;
    FOctreeBuilderWorker* Worker = nullptr;

    // 3. 스레드 동기화를 위한 잠금 장치
    FCriticalSection OctreeSwapSection;
    FCriticalSection RegisteredActorsSection;

    // 4. 추적할 액터 목록 (스레드 안전하게 접근해야 함)
    TArray<AActor*> RegisteredActors;


    // --- 에디터에서 설정할 변수들 ---
    UPROPERTY(EditAnywhere, Category = "Octree")
    FBox WorldBounds = FBox(FVector(-50000), FVector(50000));

    UPROPERTY(EditAnywhere, Category = "Octree")
    int32 MaxElementsPerNode = 10;

    UPROPERTY(EditAnywhere, Category = "Octree")
    bool bDrawDebug = true;

    UPROPERTY(EditAnywhere, Category = "Octree")
    float UpdateInterval = 0.1f;

    float TimeSinceLastUpdate = 0.0f;
};