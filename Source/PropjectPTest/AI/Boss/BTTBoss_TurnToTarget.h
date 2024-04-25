// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTBoss_TurnToTarget.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API UBTTBoss_TurnToTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTBoss_TurnToTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


};
