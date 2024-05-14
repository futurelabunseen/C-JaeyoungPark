// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PPGASInteractionItem.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Physics/PPCollision.h"
#include "AbilitySystemBlueprintLibrary.h"

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
}

UAbilitySystemComponent* APPGASInteractionItem::GetAbilitySystemComponent() const
{
	return ASC;
}

void APPGASInteractionItem::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);

	Widget->SetVisibility(true);
	//if (Widget->IsVisible())
	//{
	//	// ASC->AddLooseGameplayTag();
	//	// Interaction ���డ�� ���·� ����
	//	// �����ϸ�
	//	// �Ʒ� ��� ���
	//	if ()
	//	{
	//		InvokeGameplayCue(Other); // �����÷��� ť (�ð� ȿ�� ����)
	//		ApplyEffectToTarget(Other); // �����÷��� ����Ʈ ����

	//		// ���� ���ֱ�
	//		Mesh->SetHiddenInGame(true);
	//		SetActorEnableCollision(false);
	//		SetLifeSpan(2.0f);
	//	}
	//}
	// ���� �ȿ� ������ �±� ���̱� -> �±װ� ���� �� Interaction ������ �� �ֵ��� ����
	// -> Interaction ���� ��, �����÷��� ����Ʈ ����, ���� ����
}

void APPGASInteractionItem::NotifyActorEndOverlap(AActor* Other)
{
	Widget->SetVisibility(false);
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

