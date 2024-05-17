// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PPGA_Interaction.generated.h"

/**
 *
 */
UCLASS()
class PROPJECTPTESTGAS_API UPPGA_Interaction : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPPGA_Interaction();

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
	void ApplyEffectToTarget(AActor* Target);

	/*UFUNCTION()
	void InvokeGameplayCue(AActor* Target);*/

protected:
	UPROPERTY()
	TObjectPtr<class UAnimMontage> ActiveInteractionMontage;

	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> TargetCharacterASC;
};