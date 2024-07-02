// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/PPGA_Attack.h"
#include "Character/PPGASCharacter.h" // ĳ���� ���̽�
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h" // �����Ƽ �½�ũ, ��Ÿ��(�ִϸ��̼�)���� �� ����
#include "PropjectPTest.h"
#include "GameFramework/CharacterMovementComponent.h" // ĳ���� ������
#include "Character/PPComboActionData.h" // �޺� �׼� ������

UPPGA_Attack::UPPGA_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPPGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APPGASCharacter* PPGASCharacter = CastChecked<APPGASCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(PPGASCharacter))
	{
		// ���� �޺� ������ ��������
		CurrentComboData = PPGASCharacter->GetComboActionData();

		// ĳ���� ������ ��� ���� -> ������ ����
		PPGASCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		// ���� ���� �½�ũ
		UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayAttack"), ActionMontage, 1.0f, GetNextSection());
		if (IsValid(PlayAttackTask))
		{
			// ���� �½�ũ�� �Ϸ�Ǿ��� ��
			PlayAttackTask->OnCompleted.AddDynamic(this, &UPPGA_Attack::OnCompletedCallBack);

			// ���� �½�ũ�� ���ع޾��� ��
			PlayAttackTask->OnInterrupted.AddDynamic(this, &UPPGA_Attack::OnInterruptedCallBack);

			// ���� �½�ũ Ȱ��ȭ �غ� �Ϸ�
			PlayAttackTask->ReadyForActivation();

			// �޺� Ÿ�̸� ����
			StartComboTimer();
		}
	}
}

void UPPGA_Attack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	PPGAS_LOG(LogPPGAS, Log, TEXT("Begin"));

	// �޺� Ÿ�̸Ӱ� ������ ���� �޺� Ʈ���� false, ������ true
	if (!ComboTimerHandle.IsValid())
	{
		HasNextComboInput = false;
	}
	else
	{
		HasNextComboInput = true;
	}
}

void UPPGA_Attack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UPPGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	APPCharacter* PPCharacter = CastChecked<APPCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(PPCharacter))
	{
		// ĳ���� ������ ��� ���� -> �ȱ� ������
		PPCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		// �޺� ������ ����
		CurrentComboData = nullptr;
		CurrentCombo = 0;
		HasNextComboInput = false;
	}
}

void UPPGA_Attack::OnCompletedCallBack()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UPPGA_Attack::OnInterruptedCallBack()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

FName UPPGA_Attack::GetNextSection()
{
	// ���� �޺� ���
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, CurrentComboData->MaxComboCount);

	// ���� ���� ����
	FName NextSection = *FString::Printf(TEXT("%s%d"), *CurrentComboData->MontageSectionNamePrefix, CurrentCombo);
	return NextSection;
}

void UPPGA_Attack::StartComboTimer()
{
	// �޺� �ε���
	int32 ComboIndex = CurrentCombo - 1;
	ensure(CurrentComboData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	// �޺� ���� ���� �ð�
	const float ComboEffectiveTime = CurrentComboData->EffectiveFrameCount[ComboIndex] / CurrentComboData->FrameRate;
	if (ComboEffectiveTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this,
			&UPPGA_Attack::CheckComboInput, ComboEffectiveTime, false);
	}
}

// �޺� �Է� üũ
void UPPGA_Attack::CheckComboInput()
{
	ComboTimerHandle.Invalidate();
	if (HasNextComboInput)
	{
		MontageJumpToSection(GetNextSection());
		StartComboTimer();
		HasNextComboInput = false;
	}
}
