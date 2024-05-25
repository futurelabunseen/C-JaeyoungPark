// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/PPCharacterSkillAttributeSet.h"

UPPCharacterSkillAttributeSet::UPPCharacterSkillAttributeSet() :
	SkillRange(800.0f), // 스킬 반경
	MaxSkillRange(1200.0f), // 최대 스킬 반경

	SkillAttackRate(150.0f), // 스킬 데미지
	MaxSkillAttackRate(300.0f), // 최대 스킬 데미지

	SkillEnergy(100.0f), // 스킬 마나 소모량
	MaxSkillEnergy(100.0f) // 최대 스킬 마나 소모량
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
