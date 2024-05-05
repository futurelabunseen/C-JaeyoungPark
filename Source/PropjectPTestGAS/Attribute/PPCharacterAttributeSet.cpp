// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/PPCharacterAttributeSet.h"
#include "PropjectPTestGAS.h"
#include "GameplayEffectExtension.h"
#include "Tag/PPGameplayTag.h"
#include "Net/UnrealNetwork.h"

UPPCharacterAttributeSet::UPPCharacterAttributeSet() :
	AttackRange(100.0f), // ���� ����, OnRep ���ص� ��, ������
	MaxAttackRange(300.0f), // �ִ� ���� ����, OnRep ���ص� ��, ������

	AttackRadius(50.f), // ���� �ݰ�, OnRep ���ص� ��, ������ -> GE�� ���� ���� (Buff)
	MaxAttackRadius(150.0f), // �ִ� ���� �ݰ�, OnRep ���ص� ��, ������

	AttackRate(30.0f), // ���� ������, OnRep ���ص� ��, ������ -> GE�� ���� ���� (Buff)
	MaxAttackRate(100.0f), // �ִ� ���� ������, OnRep ���ص� ��, ������

	Health(100.0f), // ü��, OnRep �ʿ�, ���÷� ���� -> GE�� ���� ���� (Damage, Heal, Regen, Dot ���)
	MaxHealth(100.0f), // �ִ� ü��, OnRep �ʿ�

	Damage(0.0f) // �ǰ� ������, OnRep �ʿ�, ���÷� ����, �Ƹ���?
{
	InitHealth(GetMaxHealth());
}

void UPPCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterAttributeSet, Damage, COND_None, REPNOTIFY_Always);

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

// -----------------------------------------------

// OnRep Section
void UPPCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	PPGAS_LOG(LogPPGAS, Warning, TEXT("Health : %f"), GetHealth());
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterAttributeSet, Health, OldHealth);
}

void UPPCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	PPGAS_LOG(LogPPGAS, Warning, TEXT("MaxHealth : %f"), GetMaxHealth());
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterAttributeSet, MaxHealth, OldMaxHealth);
}

void UPPCharacterAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterAttributeSet, Damage, OldDamage);
}
