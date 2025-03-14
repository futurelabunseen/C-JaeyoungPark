// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "PPCharacterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOutOfHealthPlayerDelegate);

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API UPPCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPPCharacterAttributeSet();

	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, AttackRange);
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, MaxAttackRange);
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, AttackRadius);
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, MaxAttackRadius);
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, AttackRate);
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, MaxAttackRate);
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, Damage);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	mutable FOutOfHealthPlayerDelegate OnOutOfHealth_Player;

protected:

	bool bOutOfHealth = false;

	friend class UPPGE_AttackDamage;

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

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_Damage)
	FGameplayAttributeData Damage;

	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true) , ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;

// MultiPlay Section
protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_Damage(const FGameplayAttributeData& OldDamage);


};
