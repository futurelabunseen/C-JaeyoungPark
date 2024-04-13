// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "PPAT_JumpAndWaitForLanding.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpAndWaitForLandingDelegate);

/**
 * 
 */
UCLASS()
class PROPJECTPTESTGAS_API UPPAT_JumpAndWaitForLanding : public UAbilityTask
{
	GENERATED_BODY()
public:
	UPPAT_JumpAndWaitForLanding();


	// UFUNCTION �ɼ� : �������Ʈ���� ��밡��, �����Ƽ�� �½�ũ ī�װ�, �������� �̸�, ���� �Ķ����?, �⺻��?, ���ο��� �������Ʈ ��� ����
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "JumpAndWaitForLanding",
		HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UPPAT_JumpAndWaitForLanding* CreateTask(UGameplayAbility* OwningAbility);

	// Ȱ��ȭ ����
	virtual void Activate() override;

	// �ı� ����
	virtual void OnDestroy(bool AbilityEnded) override;

	// ��������Ʈ ����
	UPROPERTY(BlueprintAssignable)
	FJumpAndWaitForLandingDelegate OnComplete;

protected:
	// �ٴ� ��� ����
	UFUNCTION()
	void OnLandedCallback(const FHitResult& Hit);



};
