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
 * */
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

	// Health
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, Damage);

	// Mana (New)
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, MaxMana);

	// Experience (New)
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, Experience);
	ATTRIBUTE_ACCESSORS(UPPCharacterAttributeSet, MaxExperience);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	mutable FOutOfHealthPlayerDelegate OnOutOfHealth_Player;

protected:

	bool bOutOfHealth = false;

	friend class UPPGE_AttackDamage;

	// --- Attack Attributes ---
	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
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

	// --- Health Attributes ---
	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;

	// --- Mana Attributes (New) ---
	UPROPERTY(BlueprintReadOnly, Category = "Mana", Meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;

	UPROPERTY(BlueprintReadOnly, Category = "Mana", Meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;

	// --- Experience Attributes (New) ---
	UPROPERTY(BlueprintReadOnly, Category = "Experience", Meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_Experience)
	FGameplayAttributeData Experience;

	UPROPERTY(BlueprintReadOnly, Category = "Experience", Meta = (AllowPrivateAccess = true), ReplicatedUsing = OnRep_MaxExperience)
	FGameplayAttributeData MaxExperience;

	// MultiPlay Section
protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_Damage(const FGameplayAttributeData& OldDamage);

	// New OnRep Functions
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

	UFUNCTION()
	void OnRep_Experience(const FGameplayAttributeData& OldExperience);

	UFUNCTION()
	void OnRep_MaxExperience(const FGameplayAttributeData& OldMaxExperience);
};