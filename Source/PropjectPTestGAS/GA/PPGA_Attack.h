// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PPGA_Attack.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTESTGAS_API UPPGA_Attack : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPPGA_Attack();

public:

	// �����Ƽ Ȱ��ȭ ����
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// �Է� ������ �� ����
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	// �����Ƽ ��� ����
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	// �����Ƽ ������ ����
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	UFUNCTION()
	void OnCompletedCallBack(); // �� �� �Ϸ� ����

	UFUNCTION()
	void OnInterruptedCallBack(); // �� �� ���ع޴� ��? ����

	FName GetNextSection(); // ���� �������� ���� ����

	void StartComboTimer(); // �޺� Ÿ�̸� ���� ����
	void CheckComboInput(); // �޺� �Է� üũ ����
protected:

	UPROPERTY()
	TObjectPtr<class UPPComboActionData> CurrentComboData; // �޺� ���� ������ ����

	uint8 CurrentCombo = 0; // ���� �޺�
	FTimerHandle ComboTimerHandle; // �޺� Ÿ�̸� ����
	bool HasNextComboInput = false; // ���� �޺� �Է� ����
};
