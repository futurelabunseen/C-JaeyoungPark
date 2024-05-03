// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/MonsterAttributeSet.h"
#include "PropjectPTestGAS.h"
#include "GameplayEffectExtension.h"
#include "Tag/PPGameplayTag.h"
#include "Net/UnrealNetwork.h"

UMonsterAttributeSet::UMonsterAttributeSet() :
	AttackRange(100.0f), // ���� ����, OnRep ���ص� ��, ������
	MaxAttackRange(300.0f), // �ִ� ���� ����, OnRep ���ص� ��, ������

	AttackRadius(50.f), // ���� �ݰ�, OnRep ���ص� ��, ������
	MaxAttackRadius(150.0f), // �ִ� ���� �ݰ�, OnRep ���ص� ��, ������

	AttackRate(30.0f), // ���� ������, OnRep ���ص� ��, ������
	MaxAttackRate(100.0f), // �ִ� ���� ������, OnRep ���ص� ��, ������

	Damage(0.0f), // �ǰ� ������, OnRep �ʿ�, ���÷� ����, �Ƹ���?
	Health(100.0f), // ü��, OnRep �ʿ�, ���÷� ����
	MaxHealth(100.0f) // �ִ� ü��, OnRep �ʿ�
{
	InitHealth(GetMaxHealth());
}

void UMonsterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMonsterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMonsterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMonsterAttributeSet, Damage, COND_None, REPNOTIFY_Always);

}

void UMonsterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetDamageAttribute())
	{
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;
	}
}

bool UMonsterAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
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

void UMonsterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
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
		OnOutOfHealth_Monster.Broadcast();
	}

	bOutOfHealth = (GetHealth() <= 0.0f);
}

// -----------------------------------------------

// OnRep Section
void UMonsterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	PPGAS_LOG(LogPPGAS, Warning, TEXT("Health : %f"), GetHealth());
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMonsterAttributeSet, Health, OldHealth);
}

void UMonsterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	PPGAS_LOG(LogPPGAS, Warning, TEXT("Max Health : %f"), GetMaxHealth());
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMonsterAttributeSet, MaxHealth, OldMaxHealth);
}

void UMonsterAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMonsterAttributeSet, Damage, OldDamage);
}
