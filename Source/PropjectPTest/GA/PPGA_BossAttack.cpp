
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

	// ���� ���� �½�ũ
	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMonsterAttack"), ActionMontage, 1.0f);

	// ���� �½�ũ�� �Ϸ�Ǿ��� ��
	PlayAttackTask->OnCompleted.AddDynamic(this, &UPPGA_BossAttack::OnCompletedCallBack);

	// ���� �½�ũ�� ���ع޾��� ��
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UPPGA_BossAttack::OnInterruptedCallBack);

	// ���� �½�ũ Ȱ��ȭ �غ� �Ϸ�
	PlayAttackTask->ReadyForActivation();
}

void UPPGA_BossAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	APPGASCharacterNonPlayer* PPGASCharacterNonPlayer = CastChecked<APPGASCharacterNonPlayer>(ActorInfo->AvatarActor.Get());

	// ĳ���� ������ ��� ���� -> ������ ����
	PPGASCharacterNonPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	// DestroyAttackRange();
}

void UPPGA_BossAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	APPGASCharacterNonPlayer* PPGASCharacterNonPlayer = CastChecked<APPGASCharacterNonPlayer>(ActorInfo->AvatarActor.Get());

	// ĳ���� ������ ��� ���� -> ������ ����
	PPGASCharacterNonPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	// DestroyAttackRange();
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

//void UPPGA_BossAttack::CreateAttackRange()
//{
//	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
//	{
//		AttackRange = NewObject<USphereComponent>(Character);
//		AttackRange->InitSphereRadius(200.0f); // ���� ���� �ݰ� ����
//		AttackRange->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
//		AttackRange->SetupAttachment(Character->GetRootComponent());
//		AttackRange->OnComponentBeginOverlap.AddDynamic(this, &UPPGA_MonsterAttack::OnAttackRangeOverlap);
//		AttackRange->RegisterComponent();
//	}
//}

//void UPPGA_BossAttack::DestroyAttackRange()
//{
//	if (AttackRange)
//	{
//		AttackRange->DestroyComponent();
//		AttackRange = nullptr;
//	}
//}

//void UPPGA_BossAttack::OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor && OtherActor != GetAvatarActorFromActorInfo() && OtherActor->IsA(ACharacter::StaticClass()))
//	{
//		// ���⼭ OtherActor�� �������� �ִ� ������ �߰�
//		UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, GetOwningAbilitySystemComponent()->GetOwner(), this, UDamageType::StaticClass());
//	}
//}
