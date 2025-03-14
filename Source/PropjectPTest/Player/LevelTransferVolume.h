// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransferVolume.generated.h"

UCLASS()
class PROPJECTPTEST_API ALevelTransferVolume : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelTransferVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FString TransferLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Transfer")
	TObjectPtr<class UBoxComponent> TransferVolume;
};
