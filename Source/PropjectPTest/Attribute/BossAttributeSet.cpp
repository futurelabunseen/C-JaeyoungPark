// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/BossAttributeSet.h"
#include "PropjectPTest.h"
#include "GameplayEffectExtension.h"
#include "Tag/PPGameplayTag.h"
#include "Net/UnrealNetwork.h"

UBossAttributeSet::UBossAttributeSet() :
	AttackRange(300.0f), // ���� ����, OnRep ���ص� ��, ������
	MaxAttackRange(300.0f), // �ִ� ���� ����, OnRep ���ص� ��, ������

	AttackRadius(150.f), // ���� �ݰ�, OnRep ���ص� ��, ������
	MaxAttackRadius(150.0f), // �ִ� ���� �ݰ�, OnRep ���ص� ��, ������

	AttackRate(30.0f), // ���� ������, OnRep ���ص� ��, ������
	MaxAttackRate(100.0f), // �ִ� ���� ������, OnRep ���ص� ��, ������

	Damage(0.0f), // �ǰ� ������, OnRep �ʿ�, ���÷� ����, �Ƹ���?
	Health(100.0f), // ü��, OnRep �ʿ�, ���÷� ����
	MaxHealth(100.0f) // �ִ� ü��, OnRep �ʿ�
{
	InitHealth(GetMaxHealth());
}

void UBossAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBossAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBossAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBossAttributeSet, Damage, COND_None, REPNOTIFY_Always);

}

void UBossAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetDamageAttribute())
	{
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;
	}
}

bool UBossAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
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

void UBossAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
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
		OnOutOfHealth_Boss.Broadcast();
	}

	bOutOfHealth = (GetHealth() <= 0.0f);
}

// -----------------------------------------------

// OnRep Section
void UBossAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBossAttributeSet, Health, OldHealth);
}

void UBossAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBossAttributeSet, MaxHealth, OldMaxHealth);
}


void UBossAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBossAttributeSet, Damage, OldDamage);
}

