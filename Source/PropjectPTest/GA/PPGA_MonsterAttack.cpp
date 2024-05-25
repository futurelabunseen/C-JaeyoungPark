// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/PPGA_MonsterAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h" // �����Ƽ �½�ũ, ��Ÿ��(�ִϸ��̼�)���� �� ����
#include "PropjectPTest.h"

UPPGA_MonsterAttack::UPPGA_MonsterAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPPGA_MonsterAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// ���� ���� �½�ũ
	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMonsterAttack"), ActionMontage, 1.0f);

	// ���� �½�ũ�� �Ϸ�Ǿ��� ��
	PlayAttackTask->OnCompleted.AddDynamic(this, &UPPGA_MonsterAttack::OnCompletedCallBack);

	// ���� �½�ũ�� ���ع޾��� ��
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UPPGA_MonsterAttack::OnInterruptedCallBack);

	// ���� �½�ũ Ȱ��ȭ �غ� �Ϸ�
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
