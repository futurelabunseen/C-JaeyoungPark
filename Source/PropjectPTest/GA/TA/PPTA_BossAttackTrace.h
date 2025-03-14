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

	void DrawDebugFan(UWorld* World, const FVector& Start, const FVector& Forward, float Radius, float Angle, const FColor& Color, float Duration, bool bPersistentLines = false, uint8 DepthPriority = 0, float Thickness = 1.0f) const;

protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;

	bool bShowDebug = false;

	UPROPERTY(EditAnywhere)
	float ForwardLength = 0.0f;

	UPROPERTY(EditAnywhere)
	float AttackAngle = 0.0f;

	UPROPERTY(EditAnywhere)
	float AttackRadius = 0.0f;

};
