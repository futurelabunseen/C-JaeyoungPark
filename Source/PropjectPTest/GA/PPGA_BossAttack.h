// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PPGA_BossAttack.generated.h"

/**
 *
 */
UCLASS()
class PROPJECTPTEST_API UPPGA_BossAttack : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPPGA_BossAttack();

public:

	// 어빌리티 활성화 정의
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 어빌리티 끝내기 정의
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	UFUNCTION()
	void OnCompletedCallBack();

	UFUNCTION()
	void OnInterruptedCallBack();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ActionMontage;
};
