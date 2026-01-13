#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AI/Octree.h"
#include "AI/OctreeBuilderWorker.h"
#include "Templates/SharedPointer.h"
#include "OctreeSubsystem.generated.h"

class FRunnableThread;

UCLASS()
class PROPJECTPTEST_API UOctreeSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    // --- UWorldSubsystem 생명주기 함수 ---
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

    // OnWorldEndPlay 함수를 완전히 제거합니다.

    // --- Subsystem의 Tick 함수 ---
    void CustomTick(UWorld* World, ELevelTick TickType, float DeltaTime);

    // --- 공개 API ---
    void RegisterActor(AActor* Actor);
    void UnregisterActor(AActor* Actor);
    void FindNearbyActors(const FVector& Location, float Radius, TArray<AActor*>& OutActors);

private:
    FBox WorldBounds = FBox(FVector(-50000.0f), FVector(50000.0f));
    int32 MaxElementsPerNode = 8;
    float UpdateInterval = 1.0f;
    bool bDrawDebug = true;

    TSharedPtr<FOctreeBuilderWorker> Worker;
    FRunnableThread* WorkerThread = nullptr;

    FCriticalSection RegisteredActorsSection;
    TArray<TWeakObjectPtr<AActor>> RegisteredActors;

    FCriticalSection OctreeSwapSection;
    TSharedPtr<FGameOctree, ESPMode::ThreadSafe> ActiveOctree;
    TSharedPtr<FGameOctree, ESPMode::ThreadSafe> PendingOctree;

    float TimeSinceLastUpdate = 0.0f;

    FDelegateHandle TickDelegateHandle;
};