#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "LevelStreamerActor.generated.h"

UCLASS()
class PROPJECTPTEST_API ALevelStreamerActor : public AActor
{
    GENERATED_BODY()

public:
    ALevelStreamerActor();

protected:
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* TriggerBox;

    UPROPERTY(EditAnywhere)
    FName DungeonLevelName;

    UPROPERTY(EditAnywhere)
    FName BossLevelName;

    UPROPERTY(EditAnywhere)
    FVector BossStartLocation;

    // 타이머 핸들 (지연 언로드용)
    FTimerHandle UnloadTimerHandle;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnBossLevelLoaded();

    UFUNCTION()
    void OnBossLevelVisible();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastMovePlayers();

    UFUNCTION()
    void OnLevelUnloaded(); // 빈 껍데기 함수

    // [추가] 언로드 작업을 수행할 함수
    UFUNCTION()
    void DelayedUnloadDungeon();

    // [추가] 언로드 완료 콜백 (로그 경고 해결용)
    UFUNCTION()
    void OnDungeonUnloaded();
};