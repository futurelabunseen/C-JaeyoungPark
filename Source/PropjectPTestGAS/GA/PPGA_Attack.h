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

	// 어빌리티 활성화 정의
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 입력 눌렸을 때 정의
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	// 어빌리티 취소 정의
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	// 어빌리티 끝내기 정의
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	UFUNCTION()
	void OnCompletedCallBack(); // 콜 백 완료 정의

	UFUNCTION()
	void OnInterruptedCallBack(); // 콜 백 방해받는 중? 정의

	FName GetNextSection(); // 다음 섹션으로 가기 정의

	void StartComboTimer(); // 콤보 타이머 시작 정의
	void CheckComboInput(); // 콤보 입력 체크 정의
protected:

	UPROPERTY()
	TObjectPtr<class UPPComboActionData> CurrentComboData; // 콤보 관련 데이터 정의

	uint8 CurrentCombo = 0; // 현재 콤보
	FTimerHandle ComboTimerHandle; // 콤보 타이머 정의
	bool HasNextComboInput = false; // 다음 콤보 입력 정의
};
