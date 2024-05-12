// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PPGA_MonsterAttack.generated.h"

/**
 *
 */
UCLASS()
class PROPJECTPTESTGAS_API UPPGA_MonsterAttack : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPPGA_MonsterAttack();

public:

	// 어빌리티 활성화 정의
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 어빌리티 취소 정의
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	// 어빌리티 끝내기 정의
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	UFUNCTION()
	void OnCompletedCallBack(); // 콜 백 완료 정의

	UFUNCTION()
	void OnInterruptedCallBack(); // 콜 백 방해받는 중? 정의
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ActionMontage;
};
