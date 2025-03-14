// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "PPSkillDamageExecutionCalc.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API UPPSkillDamageExecutionCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
