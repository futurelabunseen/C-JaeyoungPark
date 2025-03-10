#include "GA/PPGA_AttackHitCheck.h"
#include "PropjectPTest.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GA/AT/PPAT_Trace.h"
#include "GA/TA/PPTA_Trace.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "Tag/PPGameplayTag.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"

// ���� ��Ʈ üũ Ŭ���� ������
UPPGA_AttackHitCheck::UPPGA_AttackHitCheck()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

// �����Ƽ Ȱ��ȭ �Լ�
void UPPGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    PPGAS_LOG(LogPPGAS, Log, TEXT("%s"), *TriggerEventData->EventTag.GetTagName().ToString());
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    CurrentLevel = TriggerEventData->EventMagnitude;

    UPPAT_Trace* AttackTraceTask = UPPAT_Trace::CreateTask(this, TargetActorClass);
    if (IsValid(AttackTraceTask))
    {
        AttackTraceTask->OnComplete.AddDynamic(this, &UPPGA_AttackHitCheck::OnTraceResultCallback);
        AttackTraceTask->ReadyForActivation();
    }
}

// GameplayTag�� �̿��� ���� ���(����)���� �Ǻ��ϴ� �Լ�
bool UPPGA_AttackHitCheck::IsEnemyTarget(AActor* SourceActor, AActor* TargetActor) const
{
    if (!SourceActor || !TargetActor)
    {
        return false;
    }

    UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
    if (!SourceASC || !TargetASC)
    {
        return false;
    }

    // �� ���Ͱ� ������ �±׸� �����ɴϴ�.
    FGameplayTagContainer SourceTags;
    FGameplayTagContainer TargetTags;
    SourceASC->GetOwnedGameplayTags(SourceTags);
    TargetASC->GetOwnedGameplayTags(TargetTags);

    // �÷��̾�� ����(����)�� �����ϴ� �±׸� �����մϴ�.
    static const FGameplayTag PlayerTag = FGameplayTag::RequestGameplayTag(FName("Character.Player"));
    static const FGameplayTag EnemyTag = FGameplayTag::RequestGameplayTag(FName("Character.Enemy"));

    // ���� ����:
    // - Source�� �÷��̾��̰� Target�� �����̸� true.
    // - �Ǵ� Source�� �����̰� Target�� �÷��̾��̸� true.
    if ((SourceTags.HasTag(PlayerTag) && TargetTags.HasTag(EnemyTag)) ||
        (SourceTags.HasTag(EnemyTag) && TargetTags.HasTag(PlayerTag)))
    {
        return true;
    }

    return false;
}

void UPPGA_AttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
    AActor* SourceActor = GetAvatarActorFromActorInfo();

    TMap<AActor*, FHitResult> UniqueTargets;

    for (int32 i = 0; i < TargetDataHandle.Data.Num(); ++i)
    {
        if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, i))
        {
            FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, i);
            AActor* TargetActor = HitResult.GetActor();
            if (TargetActor && UniqueTargets.Find(TargetActor) == nullptr)
            {
                if (IsEnemyTarget(SourceActor, TargetActor))
                {
                    UniqueTargets.Add(TargetActor, HitResult);
                }
            }
        }
        else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, i))
        {
            TArray<TWeakObjectPtr<AActor>> TargetActors = TargetDataHandle.Data[i]->GetActors();
            for (TWeakObjectPtr<AActor> WeakActor : TargetActors)
            {
                AActor* TargetActor = WeakActor.Get();
                if (TargetActor && UniqueTargets.Find(TargetActor) == nullptr)
                {
                    if (IsEnemyTarget(SourceActor, TargetActor))
                    {
                        UniqueTargets.Add(TargetActor, FHitResult());
                    }
                }
            }
        }
    }

    bool bHitDetected = false;

    for (auto& Elem : UniqueTargets)
    {
        AActor* TargetActor = Elem.Key;
        const FHitResult& HitResult = Elem.Value;

        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
        if (IsValid(TargetASC))
        {
            bHitDetected = true;

            // Ÿ�� ���� ���ο� TargetDataHandle ����
            FGameplayAbilityTargetData_ActorArray* NewTargetData = new FGameplayAbilityTargetData_ActorArray();
            NewTargetData->TargetActorArray.Add(TargetActor); // ���� �迭�� �߰�
            FGameplayAbilityTargetDataHandle NewTargetDataHandle;
            NewTargetDataHandle.Add(NewTargetData);

            if (HitResult.bBlockingHit)
            {
                ApplyEffectsToTarget(TargetASC, NewTargetDataHandle, 0, HitResult);
            }
            else
            {
                ApplyEffectsToTarget(TargetASC, NewTargetDataHandle, 0);
            }
        }
    }

    if (!bHitDetected)
    {
        UE_LOG(LogTemp, Log, TEXT("No valid ASC found in target data!"));
    }

    bool bReplicatedEndAbility = true;
    bool bWasCancelled = false;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}



// Ÿ�ٿ� ȿ�� ���� (HitResult�� �ִ� ���)
void UPPGA_AttackHitCheck::ApplyEffectsToTarget(UAbilitySystemComponent* TargetASC,
    const FGameplayAbilityTargetDataHandle& TargetDataHandle, int32 Index, const FHitResult& HitResult)
{
    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
    if (!IsValid(SourceASC) || !IsValid(TargetASC))
    {
        UE_LOG(LogTemp, Log, TEXT("ASC Not Found!"));
        return;
    }

    FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
    if (EffectSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
            EffectSpecHandle, TargetDataHandle);

        FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
        CueContextHandle.AddHitResult(HitResult);
        FGameplayCueParameters CueParam;
        CueParam.EffectContext = CueContextHandle;

        TargetASC->ExecuteGameplayCue(PPTAG_GAMEPLAYCUE_CHARACTER_ATTACKHIT, CueParam);
    }

    FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffect);
    if (BuffEffectSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle);
    }
}

// Ÿ�ٿ� ȿ�� ���� (HitResult�� ���� ���)
void UPPGA_AttackHitCheck::ApplyEffectsToTarget(UAbilitySystemComponent* TargetASC,
    const FGameplayAbilityTargetDataHandle& TargetDataHandle, int32 Index)
{
    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
    if (!IsValid(SourceASC) || !IsValid(TargetASC))
    {
        UE_LOG(LogTemp, Log, TEXT("ASC Not Found!"));
        return;
    }

    FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
    if (EffectSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
            EffectSpecHandle, TargetDataHandle);

        FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
        CueContextHandle.AddActors(TargetDataHandle.Data[Index]->GetActors(), false);
        FGameplayCueParameters CueParam;
        CueParam.EffectContext = CueContextHandle;
    }

    FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffect);
    if (BuffEffectSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle);
    }
}
