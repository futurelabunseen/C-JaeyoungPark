// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/PPGA_MonsterAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h" // 어빌리티 태스크, 몽타주(애니메이션)실행 및 중지
#include "PropjectPTest.h"

UPPGA_MonsterAttack::UPPGA_MonsterAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPPGA_MonsterAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 공격 실행 태스크
	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMonsterAttack"), ActionMontage, 1.0f);

	// 어택 태스크가 완료되었을 때
	PlayAttackTask->OnCompleted.AddDynamic(this, &UPPGA_MonsterAttack::OnCompletedCallBack);

	// 어택 태스크가 방해받았을 때
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UPPGA_MonsterAttack::OnInterruptedCallBack);

	// 어택 태스크 활성화 준비 완료
	PlayAttackTask->ReadyForActivation();
}

void UPPGA_MonsterAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UPPGA_MonsterAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPPGA_MonsterAttack::OnCompletedCallBack()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UPPGA_MonsterAttack::OnInterruptedCallBack()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
