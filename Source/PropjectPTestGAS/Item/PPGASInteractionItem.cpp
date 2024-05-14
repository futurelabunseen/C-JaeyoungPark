// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PPGASInteractionItem.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Physics/PPCollision.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Tag/PPGameplayTag.h"
#include "Character/PPGASCharacter.h"

// Sets default values
APPGASInteractionItem::APPGASInteractionItem()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	SphereTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("SphereTrigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget")); // 상호작용 UI

	RootComponent = SphereTrigger;
	Mesh->SetupAttachment(SphereTrigger);
	Widget->SetupAttachment(SphereTrigger);
	Widget->SetVisibility(false);

	SphereTrigger->SetCollisionProfileName(CPROFILE_PPTRIGGER);
	SphereTrigger->SetSphereRadius(100.0f);

	Mesh->SetRelativeLocation(FVector(0.0f, -3.5f, -30.0f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	PrimaryActorTick.bCanEverTick = true; // 틱 사용
}

UAbilitySystemComponent* APPGASInteractionItem::GetAbilitySystemComponent() const
{
	return ASC;
}

void APPGASInteractionItem::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);

	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(Other);
	Widget->SetVisibility(true);
	if (Widget->IsVisible()) // 위젯이 켜졌다면
	{
		if (!TargetCharacter) { return; }
		TargetCharacterASC = TargetCharacter->GetAbilitySystemComponent();
		TargetCharacterASC->AddLooseGameplayTag(PPTAG_CHARACTER_INTERACTION); // 태그 붙이기
	}
}

void APPGASInteractionItem::NotifyActorEndOverlap(AActor* Other)
{
	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(Other);

	Widget->SetVisibility(false);
	TargetCharacter->GetAbilitySystemComponent()->RemoveLooseGameplayTag(PPTAG_CHARACTER_INTERACTION); // 태그 없애기
}

void APPGASInteractionItem::Tick(AActor* Other)
{
	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(Other);
	if (TargetCharacterASC->HasMatchingGameplayTag(PPTAG_CHARACTER_INTERACTIONING)) // 상호작용 어빌리티가 실행되었다면
	{
		UE_LOG(LogTemp, Warning, TEXT("11111"));
		InvokeGameplayCue(TargetCharacter); // 게임플레이 큐 (시각 효과 실행)
		ApplyEffectToTarget(TargetCharacter); // 게임플레이 이펙트 적용

		UE_LOG(LogTemp, Warning, TEXT("22222"));
		// 액터 없애기
		Mesh->SetHiddenInGame(true);
		SetActorEnableCollision(false);
		SetLifeSpan(2.0f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("33333"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("44444"));
}

void APPGASInteractionItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ASC->InitAbilityActorInfo(this, this);
}

void APPGASInteractionItem::ApplyEffectToTarget(AActor* Target)
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

void APPGASInteractionItem::InvokeGameplayCue(AActor* Target)
{
	FGameplayCueParameters Param;
	Param.SourceObject = this;
	Param.Instigator = Target;
	Param.Location = GetActorLocation();
	ASC->ExecuteGameplayCue(GameplayCueTag, Param);
}

