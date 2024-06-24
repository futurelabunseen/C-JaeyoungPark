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

	UPPAT_BossAttackTrace* AttackTraceTask = UPPAT_BossAttackTrace::CreateTask(this, TargetActorClass);
	AttackTraceTask->OnComplete.AddDynamic(this, &UPPGA_BossAttackHitCheck::OnTraceResultCallback);
	AttackTraceTask->ReadyForActivation();
}

void UPPGA_BossAttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	bool bHitDetected = false;

	for (int32 i = 0; i < TargetDataHandle.Data.Num(); ++i)
	{
		if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, i))
		{
			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, i);
			PPGAS_LOG(LogPPGAS, Log, TEXT("Target %s Detected"), *(HitResult.GetActor()->GetName()));

			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());

			if (TargetASC)
			{
				bHitDetected = true;
				ApplyEffectsToTarget(TargetASC, TargetDataHandle, i, HitResult);
			}
		}
		else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, i))
		{
			TArray<TWeakObjectPtr<AActor>> TargetActors = TargetDataHandle.Data[i]->GetActors();
			for (TWeakObjectPtr<AActor> TargetActor : TargetActors)
			{
				UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor.Get());
				if (TargetASC)
				{
					bHitDetected = true;
					ApplyEffectsToTarget(TargetASC, TargetDataHandle, i);
				}
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

		FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
		CueContextHandle.AddHitResult(HitResult);
		FGameplayCueParameters CueParam;
		CueParam.EffectContext = CueContextHandle;

		// TargetASC->ExecuteGameplayCue(PPTAG_GAMEPLAYCUE_CHARACTER_ATTACKHIT, CueParam);
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

		FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
		CueContextHandle.AddActors(TargetDataHandle.Data[Index]->GetActors(), false);
		FGameplayCueParameters CueParam;
		CueParam.EffectContext = CueContextHandle;

		// SourceASC->ExecuteGameplayCue(PPTAG_GAMEPLAYCUE_CHARACTER_ATTACKHIT, CueParam);
	}

	FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffect);
	if (BuffEffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle);
	}
}
