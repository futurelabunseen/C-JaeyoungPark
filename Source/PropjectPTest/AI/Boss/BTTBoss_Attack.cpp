// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Boss/BTTBoss_Attack.h"
#include "AIController.h"
#include "Interface/BossAIInterface.h"

UBTTBoss_Attack::UBTTBoss_Attack()
{
	NodeName = TEXT("BossAttack");
}

EBTNodeResult::Type UBTTBoss_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	FAICharacterAttackFinished OnAttackFinished;
	OnAttackFinished.BindLambda(
		[&]()
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	);

	IBossAIInterface* AIPawn = Cast<IBossAIInterface>(ControllingPawn);
	if (nullptr == AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	AIPawn->SetAIAttackDelegate(OnAttackFinished);
	AIPawn->AttackByAI();
	return EBTNodeResult::InProgress;
}
