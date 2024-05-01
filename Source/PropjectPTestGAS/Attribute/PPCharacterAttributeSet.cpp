// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/PPCharacterAttributeSet.h"
#include "PropjectPTestGAS.h"
#include "GameplayEffectExtension.h"
#include "Tag/PPGameplayTag.h"
#include "Net/UnrealNetwork.h"

UPPCharacterAttributeSet::UPPCharacterAttributeSet() :
	AttackRange(100.0f),
	MaxAttackRange(300.0f),
	AttackRadius(50.f),
	MaxAttackRadius(150.0f),
	AttackRate(30.0f),
	MaxAttackRate(100.0f),
	MaxHealth(100.0f),
	Damage(0.0f)
{
	InitHealth(GetMaxHealth());
}

void UPPCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 오류 발생
	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UPPCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetDamageAttribute())
	{
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;
	}
}

bool UPPCharacterAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			if (Data.Target.HasMatchingGameplayTag(PPTAG_CHARACTER_INVINSIBLE))
			{
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
		}
	}

	return true;
}

void UPPCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.0f;

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		PPGAS_LOG(LogPPGAS, Warning, TEXT("Direct Health Access : %f"), GetHealth());
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		PPGAS_LOG(LogPPGAS, Log, TEXT("Damage : %f"), GetDamage());
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
	}

	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		Data.Target.AddLooseGameplayTag(PPTAG_CHARACTER_ISDEAD);
		OnOutOfHealth_Player.Broadcast();
	}

	bOutOfHealth = (GetHealth() <= 0.0f);
}

void UPPCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterAttributeSet, Health, OldHealth);
}

void UPPCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterAttributeSet, MaxHealth, OldMaxHealth);
}
