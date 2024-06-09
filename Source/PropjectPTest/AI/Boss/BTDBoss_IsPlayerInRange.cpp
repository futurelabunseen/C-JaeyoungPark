// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Boss/BTDBoss_IsPlayerInRange.h"
#include "AIController.h"
#include "BossAI.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDBoss_IsPlayerInRange::UBTDBoss_IsPlayerInRange()
{
	NodeName = TEXT("IsInRange");
}

bool UBTDBoss_IsPlayerInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	const APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return false;
	}

	const APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	if (nullptr == Target)
	{
		return false;
	}

	const float DistanceToTarget = ControllingPawn->GetDistanceTo(Target);	
	bResult = (DistanceToTarget <= CheckRadius);
	return bResult;
}
