// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "PPTA_BossAttackTrace.generated.h"

/**
 *
 */
UCLASS()
class PROPJECTPTEST_API APPTA_BossAttackTrace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	APPTA_BossAttackTrace();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;
	void SetShowDebug(bool InShowDebug) { bShowDebug = InShowDebug; }

protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;

	bool bShowDebug = false;
};
