
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

	// ĳ���� ������ ��� ���� -> ������ ����
	PPGASCharacterNonPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// ���� �� ȸ�� ����
	PPGASCharacterNonPlayer->bUseControllerRotationYaw = false;
	PPGASCharacterNonPlayer->GetCharacterMovement()->bOrientRotationToMovement = false;

	// ���� ���� �½�ũ
	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMonsterAttack"), ActionMontage, 1.0f);

	// ���� �½�ũ�� �Ϸ�Ǿ��� ��
	PlayAttackTask->OnCompleted.AddDynamic(this, &UPPGA_BossTeleport::OnCompletedCallBack);

	// ���� �½�ũ�� ���ع޾��� ��
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UPPGA_BossTeleport::OnInterruptedCallBack);

	// ���� �½�ũ Ȱ��ȭ �غ� �Ϸ�
	PlayAttackTask->ReadyForActivation();
}

void UPPGA_BossTeleport::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	APPGASCharacterNonPlayer* PPGASCharacterNonPlayer = CastChecked<APPGASCharacterNonPlayer>(ActorInfo->AvatarActor.Get());

	// ĳ���� ������ ��� ���� -> ������
	PPGASCharacterNonPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	// ȸ�� ��� ����
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
