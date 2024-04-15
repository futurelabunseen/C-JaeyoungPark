// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/PPGA_Interaction.h"
#include "Character/PPGASCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "PropjectPTestGAS.h"

UPPGA_Interaction::UPPGA_Interaction()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPPGA_Interaction::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(ActorInfo->AvatarActor.Get());
	if (!TargetCharacter)
	{
		return;
	}

	ActiveInteractionMontage = TargetCharacter->GetInteractionMontage();
	if (!ActiveInteractionMontage)
	{
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("InteractionMontage"), ActiveInteractionMontage, 1.0f);
	PlayMontageTask->OnCompleted.AddDynamic(this, &UPPGA_Interaction::OnCompleteCallback);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UPPGA_Interaction::OnInterruptedCallback);

	PlayMontageTask->ReadyForActivation();
}

void UPPGA_Interaction::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	PPGAS_LOG(LogPPGAS, Log, TEXT("Interaction"));
}

void UPPGA_Interaction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(ActorInfo->AvatarActor.Get());
	if (TargetCharacter)
	{
		TargetCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPPGA_Interaction::OnCompleteCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UPPGA_Interaction::OnInterruptedCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
