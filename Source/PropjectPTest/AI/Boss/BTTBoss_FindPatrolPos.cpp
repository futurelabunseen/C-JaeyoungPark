// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Boss/BTTBoss_FindPatrolPos.h"
#include "BossAI.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/BossAIInterface.h"

UBTTBoss_FindPatrolPos::UBTTBoss_FindPatrolPos()
{
	NodeName = TEXT("BossFindPatrolPos");
}

EBTNodeResult::Type UBTTBoss_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result =  Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (nullptr == NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	IBossAIInterface* AIPawn = Cast<IBossAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_HOMEPOS);
	float PatrolRadius = AIPawn->GetAIPatrolRadius();
	FNavLocation NextPatrolPos;

	if (NavSystem->GetRandomPointInNavigableRadius(Origin, PatrolRadius, NextPatrolPos))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_PATROLPOS, NextPatrolPos.Location);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;

}
