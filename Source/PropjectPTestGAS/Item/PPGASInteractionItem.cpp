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
	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget")); // ��ȣ�ۿ� UI

	RootComponent = SphereTrigger;
	Mesh->SetupAttachment(SphereTrigger);
	Widget->SetupAttachment(SphereTrigger);
	Widget->SetVisibility(false);

	SphereTrigger->SetCollisionProfileName(CPROFILE_PPTRIGGER);
	SphereTrigger->SetSphereRadius(100.0f);

	Mesh->SetRelativeLocation(FVector(0.0f, -3.5f, -30.0f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	PrimaryActorTick.bCanEverTick = true; // ƽ ���
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
	if (Widget->IsVisible()) // ������ �����ٸ�
	{
		if (!TargetCharacter) { return; }
		TargetCharacterASC = TargetCharacter->GetAbilitySystemComponent();
		TargetCharacterASC->AddLooseGameplayTag(PPTAG_CHARACTER_INTERACTION); // �±� ���̱�
	}
}

void APPGASInteractionItem::NotifyActorEndOverlap(AActor* Other)
{
	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(Other);

	Widget->SetVisibility(false);
	TargetCharacter->GetAbilitySystemComponent()->RemoveLooseGameplayTag(PPTAG_CHARACTER_INTERACTION); // �±� ���ֱ�
}

void APPGASInteractionItem::Tick(AActor* Other)
{
	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(Other);
	if (TargetCharacterASC->HasMatchingGameplayTag(PPTAG_CHARACTER_INTERACTIONING)) // ��ȣ�ۿ� �����Ƽ�� ����Ǿ��ٸ�
	{
		UE_LOG(LogTemp, Warning, TEXT("11111"));
		InvokeGameplayCue(TargetCharacter); // �����÷��� ť (�ð� ȿ�� ����)
		ApplyEffectToTarget(TargetCharacter); // �����÷��� ����Ʈ ����

		UE_LOG(LogTemp, Warning, TEXT("22222"));
		// ���� ���ֱ�
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

