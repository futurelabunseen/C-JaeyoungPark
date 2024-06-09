// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTSBoss_CheckChaseLimit.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API UBTSBoss_CheckChaseLimit : public UBTService
{
	GENERATED_BODY()

public:
	UBTSBoss_CheckChaseLimit();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
