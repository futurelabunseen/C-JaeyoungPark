// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDBoss_AttackInRange.generated.h"

/**
 *
 */
UCLASS()
class PROPJECTPTEST_API UBTDBoss_AttackInRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDBoss_AttackInRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;


};
