
#include "GA/PPGA_BossAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "PropjectPTest.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Character/PPGASCharacterNonPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

UPPGA_BossAttack::UPPGA_BossAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPPGA_BossAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	PlayAttackTask->OnCompleted.AddDynamic(this, &UPPGA_BossAttack::OnCompletedCallBack);

	// ���� �½�ũ�� ���ع޾��� ��
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UPPGA_BossAttack::OnInterruptedCallBack);

	// ���� �½�ũ Ȱ��ȭ �غ� �Ϸ�
	PlayAttackTask->ReadyForActivation();
}

void UPPGA_BossAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	APPGASCharacterNonPlayer* PPGASCharacterNonPlayer = CastChecked<APPGASCharacterNonPlayer>(ActorInfo->AvatarActor.Get());

	// ĳ���� ������ ��� ���� -> ������
	PPGASCharacterNonPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	// ȸ�� ��� ����
	PPGASCharacterNonPlayer->bUseControllerRotationYaw = true;
	PPGASCharacterNonPlayer->GetCharacterMovement()->bOrientRotationToMovement = true;
}

void UPPGA_BossAttack::OnCompletedCallBack()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UPPGA_BossAttack::OnInterruptedCallBack()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
