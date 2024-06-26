// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "ChaosRock.generated.h"

// DECLARE_DYNAMIC_DELEGATE_OneParam(FRockDestruct, FVector, RockLocation);

UCLASS()
class PROPJECTPTEST_API AChaosRock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChaosRock();

	// Called every frame
	// virtual void Tick(float DeltaTime) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRockDestruct RockDestroyDele;*/

protected:

	// Rock mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UGeometryCollectionComponent> RockMesh;

	// Box collision component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> BoxCollision;

	// Audio component for destruction sound
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UAudioComponent> DestructionSound;

	// Sound cue for the destruction sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<class USoundCue> DestructionSoundCue;

protected:

	//UPROPERTY(EditAnywhere)
	//TObjectPtr<class UBoxComponent> BoxCollision;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//TObjectPtr<class UGeometryCollectionComponent> RockMesh;

	virtual void NotifyActorBeginOverlap(class AActor* Other) override;

	/*UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);*/

private:
	// Function to handle the sound fading out
	void FadeOutSound();

	FTimerHandle FadeOutTimerHandle;
	float FadeOutDuration;
	float InitialVolume;

};
