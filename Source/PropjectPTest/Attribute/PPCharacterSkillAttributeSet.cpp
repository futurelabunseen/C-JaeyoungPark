// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/PPCharacterSkillAttributeSet.h"
#include "PropjectPTest.h"
#include "GameplayEffectExtension.h"
#include "Tag/PPGameplayTag.h"
#include "Net/UnrealNetwork.h"

UPPCharacterSkillAttributeSet::UPPCharacterSkillAttributeSet() :
	SkillRange(800.0f), // ��ų �ݰ�
	MaxSkillRange(1200.0f), // �ִ� ��ų �ݰ�

	SkillAttackRate(150.0f), // ��ų ������
	MaxSkillAttackRate(300.0f), // �ִ� ��ų ������

	SkillEnergy(100.0f), // ��ų ���� �Ҹ�
	MaxSkillEnergy(100.0f) // �ִ� ��ų ���� �Ҹ�
{
	InitSkillEnergy(GetMaxSkillEnergy());
}

void UPPCharacterSkillAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterSkillAttributeSet, SkillEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPPCharacterSkillAttributeSet, MaxSkillEnergy, COND_None, REPNOTIFY_Always);
}

void UPPCharacterSkillAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetSkillRangeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.1f, GetMaxSkillRange());
	}
	else if (Attribute == GetSkillAttackRateAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxSkillAttackRate());
	}
}

void UPPCharacterSkillAttributeSet::OnRep_SkillEnergy(const FGameplayAttributeData& OldSkillEnergy)
{
	PPGAS_LOG(LogPPGAS, Warning, TEXT("SkillEnergy : %f"), GetSkillEnergy());
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterSkillAttributeSet, SkillEnergy, OldSkillEnergy);
}

void UPPCharacterSkillAttributeSet::OnRep_MaxSkillEnergy(const FGameplayAttributeData& OldMaxSkillEnergy)
{
	PPGAS_LOG(LogPPGAS, Warning, TEXT("MaxSkillEnergy : %f"), GetMaxSkillEnergy());
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPPCharacterSkillAttributeSet, MaxSkillEnergy, OldMaxSkillEnergy);
}
