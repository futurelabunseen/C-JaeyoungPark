// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerSphere.h"
#include "Components/SphereComponent.h" // 콜리전 컴포넌트 포함
#include "HUDTriggerSphere.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API AHUDTriggerSphere : public ATriggerSphere
{
	GENERATED_BODY()
public:
	AHUDTriggerSphere();

protected:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
