// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/PPSkillDamageExecutionCalc.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PPCharacterSkillAttributeSet.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "Attribute/MonsterAttributeSet.h"
#include "Attribute/BossAttributeSet.h"

void UPPSkillDamageExecutionCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	if (IsValid(SourceASC) && IsValid(TargetASC))
	{
		AActor* SourceActor = SourceASC->GetAvatarActor();
		AActor* TargetActor = TargetASC->GetAvatarActor();

		if (IsValid(SourceActor) && IsValid(TargetActor))
		{
			const float MaxDamageRange = SourceASC->GetNumericAttributeBase(UPPCharacterSkillAttributeSet::GetSkillRangeAttribute());
			const float MaxDamage = SourceASC->GetNumericAttributeBase(UPPCharacterSkillAttributeSet::GetSkillAttackRateAttribute());
			const float Distance = FMath::Clamp(SourceActor->GetDistanceTo(TargetActor), 0.0f, MaxDamageRange);
			const float InvDamageRatio = 1.0f - Distance / MaxDamageRange;
			float Damage = InvDamageRatio * MaxDamage;

			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMonsterAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, Damage));
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBossAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, Damage));

		}
	}
}
