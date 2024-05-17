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
}

UAbilitySystemComponent* APPGASInteractionItem::GetAbilitySystemComponent() const
{
	return ASC;
}

void APPGASInteractionItem::NotifyActorBeginOverlap(AActor* Other) // 캐릭터 들어옴
{
	Super::NotifyActorBeginOverlap(Other);

	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(Other);
	if (!IsValid(TargetCharacter)) return; // 들어온 캐릭터가 플레이어가 아니면 리턴, // 유효성 검사

	TargetCharacterASC = TargetCharacter->GetAbilitySystemComponent(); // 플레이어의 ASC 가져옴
	if (!IsValid(TargetCharacterASC)) return; // ASC가 없으면 리턴, // 유효성 검사

	TargetCharacter->InteractableItem = this; // 캐릭터에게 아이템 전달
	Widget->SetVisibility(true); // 위젯 켜기

	// 위젯이 켜져있고, 상호작용 아이템 칸이 비어있지 않다면, // 유효성 검사
	if (Widget->IsVisible() && IsValid(TargetCharacter->InteractableItem))
	{
		TargetCharacterASC->AddLooseGameplayTag(PPTAG_CHARACTER_CANINTERACTION); // 상호작용 가능 태그 붙이기
	}
}

void APPGASInteractionItem::NotifyActorEndOverlap(AActor* Other) // 캐릭터가 범위를 벗어나면
{
	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(Other);

	if (!IsValid(TargetCharacter)) return; // 플레이어 아니면 리턴

	TargetCharacter->InteractableItem = NULL; // 캐릭터 아이템 삭제
	Widget->SetVisibility(false); // 위젯 없애기
	TargetCharacter->GetAbilitySystemComponent()->RemoveLooseGameplayTag(PPTAG_CHARACTER_CANINTERACTION); // 태그 없애기
}

void APPGASInteractionItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ASC->InitAbilityActorInfo(this, this);
}

