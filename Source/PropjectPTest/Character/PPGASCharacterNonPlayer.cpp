// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PPGASCharacterNonPlayer.h"
#include "AbilitySystemComponent.h"
//#include "UI/PPGASWidgetComponent.h"
//#include "UI/PPGASUserWidget.h"


APPGASCharacterNonPlayer::APPGASCharacterNonPlayer()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	ASC->SetIsReplicated(true);

	/*HpBar = CreateDefaultSubobject<UPPGASWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(200.0f, 20.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}*/
}

UAbilitySystemComponent* APPGASCharacterNonPlayer::GetAbilitySystemComponent() const
{
	return ASC;
}

void APPGASCharacterNonPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ASC->InitAbilityActorInfo(this, this);
	// UE_LOG(LogTemp, Error, TEXT("%s Monster"), *ASC->GetOwner()->GetName());

	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(InitStatEffect, Level, EffectContextHandle);
	if (EffectSpecHandle.IsValid())
	{
		ASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
	}

	for (const auto& StartAbility : StartAbilities)
	{
		FGameplayAbilitySpec StartSpec(StartAbility);
		ASC->GiveAbility(StartSpec);
	}
}

void APPGASCharacterNonPlayer::OnOutOfHealth()
{
	SetDead();
}
