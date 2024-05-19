// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChaosRock.generated.h"

// DECLARE_DYNAMIC_DELEGATE_OneParam(FRockDestruct, FVector, RockLocation);

UCLASS()
class PROPJECTPTESTGAS_API AChaosRock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChaosRock();

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRockDestruct RockDestroyDele;*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UGeometryCollectionComponent> RockMesh;

protected:

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> BoxCollision;

	virtual void NotifyActorBeginOverlap(class AActor* Other) override;

	/*UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);*/


};
