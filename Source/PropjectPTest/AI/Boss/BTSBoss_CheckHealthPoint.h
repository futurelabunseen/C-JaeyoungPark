// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTSBoss_CheckHealthPoint.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API UBTSBoss_CheckHealthPoint : public UBTService
{
	GENERATED_BODY()

public:
	UBTSBoss_CheckHealthPoint();
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	float CurrentHealthPointPercent = 0.0f;
};
