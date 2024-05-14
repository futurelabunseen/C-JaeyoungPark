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
	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget")); // 상호작용 UI

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
	//	// Interaction 실행가능 상태로 설정
	//	// 실행하면
	//	// 아래 기능 사용
	//	if ()
	//	{
	//		InvokeGameplayCue(Other); // 게임플레이 큐 (시각 효과 실행)
	//		ApplyEffectToTarget(Other); // 게임플레이 이펙트 적용

	//		// 액터 없애기
	//		Mesh->SetHiddenInGame(true);
	//		SetActorEnableCollision(false);
	//		SetLifeSpan(2.0f);
	//	}
	//}
	// 범위 안에 들어오면 태그 붙이기 -> 태그가 있을 때 Interaction 실행할 수 있도록 설정
	// -> Interaction 실행 시, 게임플레이 이펙트 적용, 액터 삭제
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

