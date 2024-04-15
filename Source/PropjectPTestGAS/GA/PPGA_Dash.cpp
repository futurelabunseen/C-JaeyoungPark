// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/PPGA_Dash.h"
#include "Character/PPGASCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "PropjectPTestGAS.h"

UPPGA_Dash::UPPGA_Dash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPPGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(ActorInfo->AvatarActor.Get());
	if (!TargetCharacter)
	{
		return;
	}

	ActiveDashActionMontage = TargetCharacter->GetDashActionMontage();
	if (!ActiveDashActionMontage)
	{
		return;
	}

	TargetCharacter->GetCharacterMovement()->AddImpulse(TargetCharacter->GetActorForwardVector()* DashMoveImpulse, true);

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("DashMontage"), ActiveDashActionMontage, 1.0f);
	PlayMontageTask->OnCompleted.AddDynamic(this, &UPPGA_Dash::OnCompleteCallback);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UPPGA_Dash::OnInterruptedCallback);

	PlayMontageTask->ReadyForActivation();
}

void UPPGA_Dash::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	PPGAS_LOG(LogPPGAS, Log, TEXT("Dash"));
}

void UPPGA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(ActorInfo->AvatarActor.Get());
	if (TargetCharacter)
	{
		TargetCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPPGA_Dash::OnCompleteCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UPPGA_Dash::OnInterruptedCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
