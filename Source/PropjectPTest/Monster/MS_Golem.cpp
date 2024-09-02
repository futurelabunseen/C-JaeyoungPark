// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/MS_Golem.h"
#include "AI/MS/MSAIController.h"
#include "Attribute/MonsterAttributeSet.h"
#include "Character/PPGASCharacter.h"
#include "GA/PPGA_Attack.h"
#include "GE/PPGE_AttackDamage.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "UI/PPGASWidgetComponent.h"
#include "UI/PPGASUserWidget.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"


// Sets default values
AMS_Golem::AMS_Golem()
{

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Simple Monster AI Setting
	AIControllerClass = AMSAIController::StaticClass();

	MonsterAttributeSet = CreateDefaultSubobject<UMonsterAttributeSet>(TEXT("MonsterAttributeSet"));

	// NonPlayer에 있던 체력바 UI 몬스터만 띄우도록 가져옴
	HpBar = CreateDefaultSubobject<UPPGASWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(200.0f, 20.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
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

