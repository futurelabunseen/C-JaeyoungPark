// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/PPGA_AttackHitCheck.h"
#include "PropjectPTestGAS.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GA/AT/PPAT_Trace.h"
#include "GA/TA/PPTA_Trace.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "Tag/PPGameplayTag.h"

UPPGA_AttackHitCheck::UPPGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPPGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	PPGAS_LOG(LogPPGAS, Log, TEXT("%s"), *TriggerEventData->EventTag.GetTagName().ToString());

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentLevel = TriggerEventData->EventMagnitude;

	UPPAT_Trace* AttackTraceTask = UPPAT_Trace::CreateTask(this, TargetActorClass);
	AttackTraceTask->OnComplete.AddDynamic(this, &UPPGA_AttackHitCheck::OnTraceResultCallback);
	AttackTraceTask->ReadyForActivation();
}

void UPPGA_AttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{

	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
	{
		FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
		PPGAS_LOG(LogPPGAS, Log, TEXT("Target %s Detected"), *(HitResult.GetActor()->GetName()));

		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());

		if (!SourceASC || !TargetASC)
		{
			UE_LOG(LogTemp, Log, TEXT("ASC Not Found!"));

			bool bReplicatedEndAbility = true;
			bool bWasCancelled = false;
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
			return;
		}

		// const UPPCharacterAttributeSet* SourceAttribute = SourceASC->GetSet<UPPCharacterAttributeSet>();

		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
		if (EffectSpecHandle.IsValid())
		{		
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

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
	else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, 0))
	{
		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();

		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
		if (EffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

			FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
			CueContextHandle.AddActors(TargetDataHandle.Data[0].Get()->GetActors(), false);
			FGameplayCueParameters CueParam;
			CueParam.EffectContext = CueContextHandle;

			SourceASC->ExecuteGameplayCue(PPTAG_GAMEPLAYCUE_CHARACTER_ATTACKHIT, CueParam);
		}
	}

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
