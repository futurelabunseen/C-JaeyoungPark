// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "PPAT_BossAttackTrace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBossAttackTraceResultDelegate, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);

/**
 *
 */
UCLASS()
class PROPJECTPTEST_API UPPAT_BossAttackTrace : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPPAT_BossAttackTrace();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitForTrace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UPPAT_BossAttackTrace* CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<class APPTA_BossAttackTrace> TargetActorClass);
	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	void SpawnAndInitializeTargetActor();
	void FinalizeTargetActor();

protected:
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle);

public:
	UPROPERTY(BlueprintAssignable)
	FBossAttackTraceResultDelegate OnComplete;

protected:
	UPROPERTY()
	TSubclassOf<class APPTA_BossAttackTrace> TargetActorClass;

	UPROPERTY()
	TObjectPtr<class APPTA_BossAttackTrace> SpawnedTargetActor;
};
