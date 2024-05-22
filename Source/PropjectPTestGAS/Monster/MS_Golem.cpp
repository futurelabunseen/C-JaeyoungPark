// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/MS_Golem.h"
#include "AI/MS/MSAIController.h"
#include "Attribute/MonsterAttributeSet.h"
#include "Character/PPGASCharacter.h"
#include "GA/PPGA_Attack.h"
#include "GE/PPGE_AttackDamage.h"
#include "AbilitySystemBlueprintLibrary.h"


// Sets default values
AMS_Golem::AMS_Golem()
{

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Simple Monster AI Setting
	AIControllerClass = AMSAIController::StaticClass();

	MonsterAttributeSet = CreateDefaultSubobject<UMonsterAttributeSet>(TEXT("MonsterAttributeSet"));

	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Ancient_Golem/Mesh/SK_Ancient_Golem.SK_Ancient_Golem'"));
	//if (CharacterMeshRef.Object)
	//{
	//	GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	//}

	//// Simple Monster AnimClass Setting
	//static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Script/Engine.AnimBlueprint'/Game/Ancient_Golem/Demo/ThirdPerson_AnimBP.ThirdPerson_AnimBP_C'"));
	//if (AnimInstanceClassRef.Class)
	//{
	//	GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	//}

	//static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Ancient_Golem/Animation/AM_Golem_Dead.AM_Golem_Dead'"));
	//if (DeadMontageRef.Object)
	//{
	//	DeadMontage = DeadMontageRef.Object;
	//}

	//static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Ancient_Golem/Animation/AM_Golem_Attack.AM_Golem_Attack'"));
	//if (AttackMontageRef.Object)
	//{
	//	AttackMontage = AttackMontageRef.Object;
	//}
}

void AMS_Golem::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	MonsterAttributeSet->OnOutOfHealth_Monster.AddDynamic(this, &ThisClass::OnOutOfHealth);
}

float AMS_Golem::GetAIPatrolRadius()
{
	return 800.0f; // 어트리뷰트 세트로 변경 예정
}

float AMS_Golem::GetAIDetectRange()
{
	return 400.0f; // 어트리뷰트 세트로 변경 예정
}

float AMS_Golem::GetAIAttackRange()
{
	return MonsterAttributeSet->GetAttackRange();
}

float AMS_Golem::GetAITurnSpeed()
{
	return 1.0f; // 어트리뷰트 세트로 변경 예정
}

void AMS_Golem::ApplyEffectToTarget(AActor* Target)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (TargetASC)
	{
		FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1, EffectContext);
		if (EffectSpecHandle.IsValid())
		{
			TargetASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
		}
	}
}

void AMS_Golem::InvokeGameplayCue(AActor* Target)
{
	FGameplayCueParameters Param;
	Param.SourceObject = this;
	Param.Instigator = Target;
	Param.Location = GetActorLocation();
	ASC->ExecuteGameplayCue(GameplayCueTag, Param);
}

