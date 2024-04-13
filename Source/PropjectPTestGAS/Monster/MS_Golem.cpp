// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/MS_Golem.h"
#include "AI/MSAIController.h"
#include "Attribute/PPCharacterAttributeSet.h"


// Sets default values
AMS_Golem::AMS_Golem()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Simple Monster AI Setting
	AIControllerClass = AMSAIController::StaticClass();

	// Simple Monster AnimClass Setting
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Script/Engine.AnimBlueprint'/Game/Ancient_Golem/Demo/ThirdPerson_AnimBP.ThirdPerson_AnimBP_C'"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Ancient_Golem/Animation/AM_Golem_Dead.AM_Golem_Dead'"));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Ancient_Golem/Animation/AM_Golem_Attack.AM_Golem_Attack'"));
	if (AttackMontageRef.Object)
	{
		AttackMontage = AttackMontageRef.Object;
	}
}

float AMS_Golem::GetAIPatrolRadius()
{
	return 800.0f;
}

float AMS_Golem::GetAIDetectRange()
{
	return 1000.0f;
}

float AMS_Golem::GetAIAttackRange()
{
	return AttributeSet->GetAttackRange();
}

float AMS_Golem::GetAITurnSpeed()
{
	return 2.0f;
}

void AMS_Golem::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AMS_Golem::AttackByAI()
{
	PlayAnimMontage(AttackMontage);
	// 예제와 구조가 다름
	// 캐릭터 베이스에서 구조 변경 후 적용
}

//void AMS_Golem::NotifyComboActionEnd()
//{
//	Super::NotifyComboActionEnd();
//	OnAttackFinished.ExecuteIfBound();
//}

