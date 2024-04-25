// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTSBoss_Detect.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API UBTSBoss_Detect : public UBTService
{
	GENERATED_BODY()
public:
	UBTSBoss_Detect();

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


};
