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
#include "AI/OctreeSubsystem.h" 
#include "Kismet/GameplayStatics.h"


// Sets default values
AMS_Golem::AMS_Golem()
{
	Tags.Add(FName("Monster"));

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

void AMS_Golem::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		UOctreeSubsystem* OctreeSubsystem = World->GetSubsystem<UOctreeSubsystem>();
		if (OctreeSubsystem)
		{
			OctreeSubsystem->RegisterActor(this);
		}
	}
}

void AMS_Golem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason); // 부모 클래스의 EndPlay를 호출합니다.

	UWorld* World = GetWorld();
	if (World)
	{
		UOctreeSubsystem* OctreeSubsystem = World->GetSubsystem<UOctreeSubsystem>();
		if (OctreeSubsystem)
		{
			OctreeSubsystem->UnregisterActor(this);
		}
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

void AMS_Golem::OnOutOfHealth(AActor* Killer)
{
	// 1. 이미 죽은 상태 처리 (UI 끄기, 충돌 끄기 등 기존 로직)
	if (HpBar)
	{
		HpBar->SetHiddenInGame(true);
	}
	SetActorEnableCollision(false);

	// 2. 킬러에게 경험치 보상 지급
	if (Killer && ExpRewardEffectClass)
	{
		// 킬러의 ASC(어빌리티 시스템) 가져오기
		UAbilitySystemComponent* KillerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Killer);
		if (KillerASC)
		{
			// 이펙트 컨텍스트 및 스펙 생성
			FGameplayEffectContextHandle EffectContext = KillerASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle EffectSpecHandle = KillerASC->MakeOutgoingSpec(ExpRewardEffectClass, 1.0f, EffectContext);

			if (EffectSpecHandle.IsValid())
			{
				// ★ 중요: SetByCaller를 통해 경험치 양을 동적으로 설정
				// "Data.Reward.Exp" 태그는 BPGE_ExpReward에서 설정한 Data Tag와 일치해야 합니다.
				EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Reward.Exp")), ExpRewardAmount);

				// 킬러에게 적용
				KillerASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
			}
		}
	}

	// 3. 사망 처리 (파괴 혹은 래그돌)
	// 예시: 3초 뒤 액터 제거
	SetLifeSpan(3.0f);

	// 만약 부모 클래스(APPGASCharacterNonPlayer)에 OnOutOfHealth 로직이 있다면 호출 필요할 수 있음
	// Super::OnOutOfHealth(); // (구조에 따라 필요시 추가)
}

