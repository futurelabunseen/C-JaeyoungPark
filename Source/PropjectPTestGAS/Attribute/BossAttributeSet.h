// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BossAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOutOfHealthBossDelegate);

/**
 * 
 */
UCLASS()
class PROPJECTPTESTGAS_API UBossAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UBossAttributeSet();

	ATTRIBUTE_ACCESSORS(UBossAttributeSet, AttackRange);
	ATTRIBUTE_ACCESSORS(UBossAttributeSet, MaxAttackRange);
	ATTRIBUTE_ACCESSORS(UBossAttributeSet, AttackRadius);
	ATTRIBUTE_ACCESSORS(UBossAttributeSet, MaxAttackRadius);
	ATTRIBUTE_ACCESSORS(UBossAttributeSet, AttackRate);
	ATTRIBUTE_ACCESSORS(UBossAttributeSet, MaxAttackRate);
	ATTRIBUTE_ACCESSORS(UBossAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UBossAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UBossAttributeSet, Damage);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	mutable FOutOfHealthBossDelegate OnOutOfHealth_Boss;

protected:
	UPROPERTY(BlueprintReadOnly, Category="Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackRange;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAttackRange;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackRadius;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAttackRadius;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackRate;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAttackRate;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	bool bOutOfHealth = false;

	friend class UPPGE_AttackDamage;
};
