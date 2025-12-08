// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "PPCharacterSkillAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 스킬 고유의 속성(사거리, 데미지 계수 등)만 관리합니다.
 * 자원 소모(Mana)는 PPCharacterAttributeSet에서 관리합니다.
 */
UCLASS()
class PROPJECTPTEST_API UPPCharacterSkillAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPPCharacterSkillAttributeSet();

	ATTRIBUTE_ACCESSORS(UPPCharacterSkillAttributeSet, SkillRange);
	ATTRIBUTE_ACCESSORS(UPPCharacterSkillAttributeSet, MaxSkillRange);
	ATTRIBUTE_ACCESSORS(UPPCharacterSkillAttributeSet, SkillAttackRate);
	ATTRIBUTE_ACCESSORS(UPPCharacterSkillAttributeSet, MaxSkillAttackRate);
	// SkillEnergy, MaxSkillEnergy 삭제됨

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SkillRange;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSkillRange;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SkillAttackRate;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSkillAttackRate;

	// SkillEnergy 관련 변수 및 OnRep 함수 삭제됨
};