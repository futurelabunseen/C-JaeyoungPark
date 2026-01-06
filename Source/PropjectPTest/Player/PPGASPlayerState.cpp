#include "Player/PPGASPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "Attribute/PPCharacterSkillAttributeSet.h"

APPGASPlayerState::APPGASPlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UPPCharacterAttributeSet>(TEXT("AttributeSet"));
	SkillAttributeSet = CreateDefaultSubobject<UPPCharacterSkillAttributeSet>(TEXT("SkillAttributeSet"));
	NetUpdateFrequency = 50.0f;
}

UAbilitySystemComponent* APPGASPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}