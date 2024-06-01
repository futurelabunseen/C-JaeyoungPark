// Fill out your copyright notice in the Description page of Project Settings.

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
	// Sets default values for this actor's properties
	ALevelStreamerActor();

	// 게임 시작 또는 스폰 시 호출됨
	virtual void BeginPlay() override;

	// 프레임마다 호출됨
	virtual void Tick(float DeltaSeconds) override;

protected:
	UFUNCTION()
	void OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void MovePlayerToStart();

	UFUNCTION()
	void OnLevelLoaded();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> OverlapVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName LevelToLoad = "_mansion_";

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName LevelToUnLoad;*/
};
