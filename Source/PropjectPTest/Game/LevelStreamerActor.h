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

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnBossLevelLoaded();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastMovePlayers();
};