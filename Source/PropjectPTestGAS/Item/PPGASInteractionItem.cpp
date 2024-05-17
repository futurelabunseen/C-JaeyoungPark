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
}

UAbilitySystemComponent* APPGASInteractionItem::GetAbilitySystemComponent() const
{
	return ASC;
}

void APPGASInteractionItem::NotifyActorBeginOverlap(AActor* Other) // ĳ���� ����
{
	Super::NotifyActorBeginOverlap(Other);

	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(Other);
	if (!IsValid(TargetCharacter)) return; // ���� ĳ���Ͱ� �÷��̾ �ƴϸ� ����, // ��ȿ�� �˻�

	TargetCharacterASC = TargetCharacter->GetAbilitySystemComponent(); // �÷��̾��� ASC ������
	if (!IsValid(TargetCharacterASC)) return; // ASC�� ������ ����, // ��ȿ�� �˻�

	TargetCharacter->InteractableItem = this; // ĳ���Ϳ��� ������ ����
	Widget->SetVisibility(true); // ���� �ѱ�

	// ������ �����ְ�, ��ȣ�ۿ� ������ ĭ�� ������� �ʴٸ�, // ��ȿ�� �˻�
	if (Widget->IsVisible() && IsValid(TargetCharacter->InteractableItem))
	{
		TargetCharacterASC->AddLooseGameplayTag(PPTAG_CHARACTER_CANINTERACTION); // ��ȣ�ۿ� ���� �±� ���̱�
	}
}

void APPGASInteractionItem::NotifyActorEndOverlap(AActor* Other) // ĳ���Ͱ� ������ �����
{
	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(Other);

	if (!IsValid(TargetCharacter)) return; // �÷��̾� �ƴϸ� ����

	TargetCharacter->InteractableItem = NULL; // ĳ���� ������ ����
	Widget->SetVisibility(false); // ���� ���ֱ�
	TargetCharacter->GetAbilitySystemComponent()->RemoveLooseGameplayTag(PPTAG_CHARACTER_CANINTERACTION); // �±� ���ֱ�
}

void APPGASInteractionItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ASC->InitAbilityActorInfo(this, this);
}

