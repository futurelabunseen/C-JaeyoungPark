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

	// �����Ƽ Ȱ��ȭ ����
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// �����Ƽ ��� ����
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	// �����Ƽ ������ ����
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	UFUNCTION()
	void OnCompletedCallBack(); // �� �� �Ϸ� ����

	UFUNCTION()
	void OnInterruptedCallBack(); // �� �� ���ع޴� ��? ����
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ActionMontage;
};
