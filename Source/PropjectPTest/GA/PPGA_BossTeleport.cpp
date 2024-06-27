
#include "GA/PPGA_BossTeleport.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "PropjectPTest.h"
#include "GameFramework/Character.h"
#include "Character/PPGASCharacterNonPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

UPPGA_BossTeleport::UPPGA_BossTeleport()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPPGA_BossTeleport::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APPGASCharacterNonPlayer* PPGASCharacterNonPlayer = CastChecked<APPGASCharacterNonPlayer>(ActorInfo->AvatarActor.Get());

	// 캐릭터 움직임 모드 정의 -> 움직임 없음
	PPGASCharacterNonPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// 시전 중 회전 고정
	PPGASCharacterNonPlayer->bUseControllerRotationYaw = false;
	PPGASCharacterNonPlayer->GetCharacterMovement()->bOrientRotationToMovement = false;

	// 공격 실행 태스크
	UAbilityTask_PlayMontageAndWait* PlayTeleportTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayTeleport"), ActionMontage, 1.0f);

	// 어택 태스크가 완료되었을 때
	PlayTeleportTask->OnCompleted.AddDynamic(this, &UPPGA_BossTeleport::OnCompletedCallBack);

	// 어택 태스크가 방해받았을 때
	PlayTeleportTask->OnInterrupted.AddDynamic(this, &UPPGA_BossTeleport::OnInterruptedCallBack);

	// 어택 태스크 활성화 준비 완료
	PlayTeleportTask->ReadyForActivation();
}

void UPPGA_BossTeleport::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	APPGASCharacterNonPlayer* PPGASCharacterNonPlayer = CastChecked<APPGASCharacterNonPlayer>(ActorInfo->AvatarActor.Get());

	// 캐릭터 움직임 모드 정의 -> 움직임
	PPGASCharacterNonPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	// 회전 잠금 해제
	PPGASCharacterNonPlayer->bUseControllerRotationYaw = true;
	PPGASCharacterNonPlayer->GetCharacterMovement()->bOrientRotationToMovement = true;
}

void UPPGA_BossTeleport::OnCompletedCallBack()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UPPGA_BossTeleport::OnInterruptedCallBack()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
