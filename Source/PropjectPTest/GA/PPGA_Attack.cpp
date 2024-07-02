// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/PPGA_Attack.h"
#include "Character/PPGASCharacter.h" // 캐릭터 베이스
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h" // 어빌리티 태스크, 몽타주(애니메이션)실행 및 중지
#include "PropjectPTest.h"
#include "GameFramework/CharacterMovementComponent.h" // 캐릭터 움직임
#include "Character/PPComboActionData.h" // 콤보 액션 데이터

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
		// 현재 콤보 데이터 가져오기
		CurrentComboData = PPGASCharacter->GetComboActionData();

		// 캐릭터 움직임 모드 정의 -> 움직임 없음
		PPGASCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		// 공격 실행 태스크
		UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayAttack"), ActionMontage, 1.0f, GetNextSection());
		if (IsValid(PlayAttackTask))
		{
			// 어택 태스크가 완료되었을 때
			PlayAttackTask->OnCompleted.AddDynamic(this, &UPPGA_Attack::OnCompletedCallBack);

			// 어택 태스크가 방해받았을 때
			PlayAttackTask->OnInterrupted.AddDynamic(this, &UPPGA_Attack::OnInterruptedCallBack);

			// 어택 태스크 활성화 준비 완료
			PlayAttackTask->ReadyForActivation();

			// 콤보 타이머 시작
			StartComboTimer();
		}
	}
}

void UPPGA_Attack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	PPGAS_LOG(LogPPGAS, Log, TEXT("Begin"));

	// 콤보 타이머가 없으면 다음 콤보 트리거 false, 있으면 true
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
		// 캐릭터 움직임 모드 정의 -> 걷기 움직임
		PPCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		// 콤보 데이터 비우기
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
	// 현재 콤보 계산
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, CurrentComboData->MaxComboCount);

	// 다음 섹션 정의
	FName NextSection = *FString::Printf(TEXT("%s%d"), *CurrentComboData->MontageSectionNamePrefix, CurrentCombo);
	return NextSection;
}

void UPPGA_Attack::StartComboTimer()
{
	// 콤보 인덱스
	int32 ComboIndex = CurrentCombo - 1;
	ensure(CurrentComboData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	// 콤보 실행 가능 시간
	const float ComboEffectiveTime = CurrentComboData->EffectiveFrameCount[ComboIndex] / CurrentComboData->FrameRate;
	if (ComboEffectiveTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this,
			&UPPGA_Attack::CheckComboInput, ComboEffectiveTime, false);
	}
}

// 콤보 입력 체크
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
