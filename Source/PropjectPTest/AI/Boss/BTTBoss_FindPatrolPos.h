// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTBoss_FindPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API UBTTBoss_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTBoss_FindPatrolPos();


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};