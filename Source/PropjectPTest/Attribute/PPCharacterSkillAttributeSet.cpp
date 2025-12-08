// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/PPCharacterSkillAttributeSet.h"
#include "PropjectPTest.h"
#include "GameplayEffectExtension.h"
#include "Tag/PPGameplayTag.h"
#include "Net/UnrealNetwork.h"

UPPCharacterSkillAttributeSet::UPPCharacterSkillAttributeSet() :
	SkillRange(800.0f),
	MaxSkillRange(1200.0f),

	SkillAttackRate(150.0f),
	MaxSkillAttackRate(300.0f)
	// SkillEnergy 초기화 삭제
{
	// InitSkillEnergy 삭제
}

// GetLifetimeReplicatedProps 함수는 리플리케이트 할 변수가 없다면 생략 가능하지만, 
// 추후 확장을 위해 남겨둔다면 아래처럼 비워두거나 함수 자체를 지워도 됨.
// 여기서는 리플리케이션 할 변수가 현재 없으므로 함수 자체를 제거해도 무방함.

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

// OnRep_SkillEnergy 관련 함수들 삭제됨