// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Boss_Mermaid.h"
#include "AI/Boss/BossAIController.h"
#include "Attribute/BossAttributeSet.h"
#include "Character/PPGASCharacter.h"


// Sets default values
ABoss_Mermaid::ABoss_Mermaid()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Boss AI Setting
	AIControllerClass = ABossAIController::StaticClass();

	BossAttributeSet = CreateDefaultSubobject<UBossAttributeSet>(TEXT("BossAttributeSet"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Mermaid/Mesh/MermaidVersion/SK_Mermaid.SK_Mermaid'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// Boss AnimClass Setting
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Script/Engine.AnimBlueprint'/Game/Mermaid/Animations/ThirdPerson/ThirdPerson_AnimBP.ThirdPerson_AnimBP_C'"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Mermaid/Animations/Custom_Mermaid/Mermaid_Dead.Mermaid_Dead'"));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Mermaid/Animations/Custom_Mermaid/Mermaid_Attack.Mermaid_Attack'"));
	if (AttackMontageRef.Object)
	{
		AttackMontage = AttackMontageRef.Object;
	}
}

void ABoss_Mermaid::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	BossAttributeSet->OnOutOfHealth_Boss.AddDynamic(this, &ThisClass::OnOutOfHealth);
}

float ABoss_Mermaid::GetAIPatrolRadius()
{
	return 800.0f;
}

float ABoss_Mermaid::GetAIDetectRange()
{
	return 1000.0f;
}

float ABoss_Mermaid::GetAIAttackRange()
{
	return BossAttributeSet->GetAttackRange();
}

float ABoss_Mermaid::GetAITurnSpeed()
{
	return 2.0f;
}

void ABoss_Mermaid::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void ABoss_Mermaid::AttackByAI()
{
	//PlayAnimMontage(AttackMontage);
	// 예제와 구조가 다름
	// 캐릭터 베이스에서 구조 변경 후 적용

}

//void AMS_Golem::NotifyComboActionEnd()
//{
//	Super::NotifyComboActionEnd();
//	OnAttackFinished.ExecuteIfBound();
//}

