// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PPGA_Dash.generated.h"

/**
 *
 */
UCLASS()
class PROPJECTPTEST_API UPPGA_Dash : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPPGA_Dash();

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnCompleteCallback();

	UFUNCTION()
	void OnInterruptedCallback();

	UFUNCTION()
	void OnCancelledCallback();

protected:
	UPROPERTY()
	TObjectPtr<class UAnimMontage> ActiveDashActionMontage;

	UPROPERTY(EditAnywhere, Category = Move)
	float DashMoveImpulse = 10000.0f;
};
