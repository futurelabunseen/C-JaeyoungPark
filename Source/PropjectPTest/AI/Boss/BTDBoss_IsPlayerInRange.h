// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDBoss_IsPlayerInRange.generated.h"


UCLASS()
class PROPJECTPTEST_API UBTDBoss_IsPlayerInRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDBoss_IsPlayerInRange();

	UPROPERTY(EditAnywhere)
	float CheckRadius;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
