// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/PPGA_Jump.h"
#include "GameFramework/Character.h"
#include "GA/AT/PPAT_JumpAndWaitForLanding.h"

UPPGA_Jump::UPPGA_Jump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; // ���� �� ��ü ��å ����
}



bool UPPGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	if (!bResult)
	{
		return false;
	}

	const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	return (Character && Character->CanJump()); // ĳ���Ͱ� �ְ�, ������ �� �ִ� ���� �϶�
}




void UPPGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}



// �Է��� ���� ��
void UPPGA_Jump::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->StopJumping(); // ���� �׸�
}

// �ٴڿ� ����� ��
void UPPGA_Jump::OnLandedCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;

	// �����Ƽ ������
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
