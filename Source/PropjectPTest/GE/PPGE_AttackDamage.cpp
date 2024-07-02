// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/PPGE_AttackDamage.h"

UPPGE_AttackDamage::UPPGE_AttackDamage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo HealthModifier;
	HealthModifier.ModifierOp = EGameplayModOp::Additive;

	FScalableFloat DamageAmount(-30.0f);
	FGameplayEffectModifierMagnitude ModMagnitude(DamageAmount);

	HealthModifier.ModifierMagnitude = ModMagnitude;
	Modifiers.Add(HealthModifier);
}
