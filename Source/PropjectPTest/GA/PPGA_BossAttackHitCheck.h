// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PPGA_BossAttackHitCheck.generated.h"

/**
 *
 */
UCLASS()
class PROPJECTPTEST_API UPPGA_BossAttackHitCheck : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UPPGA_BossAttackHitCheck();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
    UFUNCTION()
    void OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

    void ApplyEffectsToTarget(UAbilitySystemComponent* TargetASC, const FGameplayAbilityTargetDataHandle& TargetDataHandle, int32 Index, const FHitResult& HitResult);

    void ApplyEffectsToTarget(UAbilitySystemComponent* TargetASC, const FGameplayAbilityTargetDataHandle& TargetDataHandle, int32 Index);

    UPROPERTY(EditAnywhere, Category = "GAS")
    TSubclassOf<class UGameplayEffect> AttackDamageEffect;

    UPROPERTY(EditAnywhere, Category = "GAS")
    TSubclassOf<class UGameplayEffect> AttackBuffEffect;

    float CurrentLevel;

    UPROPERTY(EditAnywhere, Category = "GAS")
    TSubclassOf<class APPTA_BossAttackTrace> TargetActorClass;

private:
    TSet<AActor*> UniqueHitActors; // 멤버 변수로 선언
};