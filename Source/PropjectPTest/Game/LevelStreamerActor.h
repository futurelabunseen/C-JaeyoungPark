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
    // 트리거 박스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LevelStreamer")
    UBoxComponent* TriggerBox;

    // 에디터에서 수정 가능한 레벨 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelStreamer")
    FName DungeonLevelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelStreamer")
    FName BossLevelName;

    // 보스 방 이동 좌표
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelStreamer")
    FVector BossStartLocation;

    // 중복 로딩 방지 플래그
    bool bIsLoading = false;

    FTimerHandle UnloadTimerHandle;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnBossLevelLoaded();

    UFUNCTION()
    void OnBossLevelVisible();

    UFUNCTION()
    void DelayedUnloadDungeon();

    UFUNCTION()
    void OnDungeonUnloaded();

    void MoveAllPlayersToBossRoom();
};