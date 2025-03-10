#include "GA/PPGA_BossAttackHitCheck.h"
#include "PropjectPTest.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GA/AT/PPAT_BossAttackTrace.h"
#include "GA/TA/PPTA_BossAttackTrace.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "Tag/PPGameplayTag.h"

UPPGA_BossAttackHitCheck::UPPGA_BossAttackHitCheck()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPPGA_BossAttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    PPGAS_LOG(LogPPGAS, Log, TEXT("%s"), *TriggerEventData->EventTag.GetTagName().ToString());

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    CurrentLevel = TriggerEventData->EventMagnitude;

    // 공격 전 멤버 변수 초기화
    UniqueHitActors.Empty(); // 중복을 방지하기 위해 초기화

    UPPAT_BossAttackTrace* AttackTraceTask = UPPAT_BossAttackTrace::CreateTask(this, TargetActorClass);
    AttackTraceTask->OnComplete.AddDynamic(this, &UPPGA_BossAttackHitCheck::OnTraceResultCallback);
    AttackTraceTask->ReadyForActivation();
}

void UPPGA_BossAttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
    AActor* SourceActor = GetAvatarActorFromActorInfo();

    UniqueHitActors.Empty(); // 매 공격마다 초기화

    for (int32 i = 0; i < TargetDataHandle.Data.Num(); ++i)
    {
        if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, i))
        {
            FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, i);
            AActor* TargetActor = HitResult.GetActor();

            if (TargetActor && !UniqueHitActors.Contains(TargetActor)) // 중복 체크
            {
                UniqueHitActors.Add(TargetActor);

                UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
                if (IsValid(TargetASC))
                {
                    FGameplayAbilityTargetDataHandle NewTargetDataHandle;
                    FGameplayAbilityTargetData_ActorArray* NewTargetData = new FGameplayAbilityTargetData_ActorArray();
                    NewTargetData->TargetActorArray.Add(TargetActor);
                    NewTargetDataHandle.Add(NewTargetData);

                    ApplyEffectsToTarget(TargetASC, NewTargetDataHandle, 0, HitResult);
                }
            }
        }
        else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, i))
        {
            TArray<TWeakObjectPtr<AActor>> TargetActors = TargetDataHandle.Data[i]->GetActors();
            for (TWeakObjectPtr<AActor> WeakActor : TargetActors)
            {
                AActor* TargetActor = WeakActor.Get();

                if (TargetActor && !UniqueHitActors.Contains(TargetActor)) // 중복 체크
                {
                    UniqueHitActors.Add(TargetActor);

                    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
                    if (IsValid(TargetASC))
                    {
                        FGameplayAbilityTargetDataHandle NewTargetDataHandle;
                        FGameplayAbilityTargetData_ActorArray* NewTargetData = new FGameplayAbilityTargetData_ActorArray();
                        NewTargetData->TargetActorArray.Add(TargetActor);
                        NewTargetDataHandle.Add(NewTargetData);

                        ApplyEffectsToTarget(TargetASC, NewTargetDataHandle, 0);
                    }
                }
            }
        }
    }

    bool bReplicatedEndAbility = true;
    bool bWasCancelled = false;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}



void UPPGA_BossAttackHitCheck::ApplyEffectsToTarget(UAbilitySystemComponent* TargetASC, const FGameplayAbilityTargetDataHandle& TargetDataHandle, int32 Index, const FHitResult& HitResult)
{
    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
    if (!SourceASC || !TargetASC)
    {
        UE_LOG(LogTemp, Log, TEXT("ASC Not Found!"));
        return;
    }

    FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
    if (EffectSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
    }

    FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffect);
    if (BuffEffectSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle);
    }
}

void UPPGA_BossAttackHitCheck::ApplyEffectsToTarget(UAbilitySystemComponent* TargetASC, const FGameplayAbilityTargetDataHandle& TargetDataHandle, int32 Index)
{
    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
    if (!SourceASC || !TargetASC)
    {
        UE_LOG(LogTemp, Log, TEXT("ASC Not Found!"));
        return;
    }

    FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
    if (EffectSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
    }

    FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffect);
    if (BuffEffectSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle);
    }
}