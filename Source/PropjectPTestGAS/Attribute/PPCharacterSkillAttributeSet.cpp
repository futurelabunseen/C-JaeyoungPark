// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/PPCharacterSkillAttributeSet.h"

UPPCharacterSkillAttributeSet::UPPCharacterSkillAttributeSet() :
	SkillRange(800.0f), // ��ų �ݰ�
	MaxSkillRange(1200.0f), // �ִ� ��ų �ݰ�

	SkillAttackRate(150.0f), // ��ų ������
	MaxSkillAttackRate(300.0f), // �ִ� ��ų ������

	SkillEnergy(100.0f), // ��ų ���� �Ҹ�
	MaxSkillEnergy(100.0f) // �ִ� ��ų ���� �Ҹ�
{
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
