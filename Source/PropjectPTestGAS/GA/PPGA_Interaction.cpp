// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/PPGA_Interaction.h"
#include "Character/PPGASCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "PropjectPTestGAS.h"
#include "Tag/PPGameplayTag.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Item/PPGASInteractionItem.h"
#include "Kismet/GameplayStatics.h"


UPPGA_Interaction::UPPGA_Interaction()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPPGA_Interaction::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(ActorInfo->AvatarActor.Get()); // ���� �÷��̾� ĳ����
	APPGASInteractionItem* InteractableItem = Cast<APPGASInteractionItem>(TargetCharacter->InteractableItem); // ���� ��ȣ�ۿ� ������
	if (!IsValid(TargetCharacter)) // ��ȿ�� �˻�
	{
		return;
	}

	if (!IsValid(InteractableItem)) // ��ȿ�� �˻�
	{
		return;
	}

	ActiveInteractionMontage = TargetCharacter->GetInteractionMontage();
	if (!ActiveInteractionMontage) // ��ȿ�� �˻�
	{
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("InteractionMontage"), ActiveInteractionMontage, 1.0f);
	if (IsValid(PlayMontageTask)) // ��ȿ�� �˻�
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &UPPGA_Interaction::OnCompleteCallback);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UPPGA_Interaction::OnInterruptedCallback);

		PlayMontageTask->ReadyForActivation();
	}

	TargetCharacterASC = TargetCharacter->GetAbilitySystemComponent(); // �÷��̾� ASC
	if (!TargetCharacterASC) // ��ȿ�� �˻�
	{
		return;
	}

	if (TargetCharacterASC->HasMatchingGameplayTag(PPTAG_CHARACTER_INTERACTIONING)) // ��ȣ�ۿ� �����Ƽ�� ����Ǿ��ٸ�
	{
		// InvokeGameplayCue(TargetCharacter); // �����÷��� ť (�ð� ȿ�� ����)
		ApplyEffectToTarget(TargetCharacter); // �����÷��� ����Ʈ ����

		UWorld* World = GetWorld();
		SpawnParticleEffect(World, TargetCharacter->GetActorLocation(), TargetCharacter->GetActorRotation(), InteractableItem->ParticleSystem);

		// ���� ���ֱ�
		InteractableItem->Mesh->SetHiddenInGame(true);
		InteractableItem->SetActorEnableCollision(false);
		InteractableItem->SetLifeSpan(2.0f);
	}
	else
	{
		return;
	}
}

void UPPGA_Interaction::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	PPGAS_LOG(LogPPGAS, Log, TEXT("Interaction"));
}

void UPPGA_Interaction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(TargetCharacter)) // ��ȿ�� �˻�
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

void UPPGA_Interaction::ApplyEffectToTarget(AActor* Target)
{
	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(Target);
	if (TargetCharacterASC && IsValid(TargetCharacter)) // ��ȿ�� �˻�
	{
		APPGASInteractionItem* InteractableItem = Cast<APPGASInteractionItem>(TargetCharacter->InteractableItem);
		if (IsValid(InteractableItem) && InteractableItem->GameplayEffectClass) // ��ȿ�� �˻�
		{
			FGameplayEffectContextHandle EffectContext = TargetCharacterASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			FGameplayEffectSpecHandle EffectSpecHandle = TargetCharacterASC->MakeOutgoingSpec(InteractableItem->GameplayEffectClass, 1, EffectContext);
			if (EffectSpecHandle.IsValid()) // ��ȿ�� �˻�
			{
				TargetCharacterASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
			}
		}
	}
}

void UPPGA_Interaction::SpawnParticleEffect(UWorld* World, FVector Location, FRotator Rotation, UParticleSystem* ParticleSystem)
{
	UGameplayStatics::SpawnEmitterAtLocation(World, ParticleSystem, Location, Rotation, true);
}

//void UPPGA_Interaction::InvokeGameplayCue(AActor* Target)
//{
//	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(Target);
//	APPGASInteractionItem* InteractableItem = Cast<APPGASInteractionItem>(TargetCharacter->InteractableItem);
//	if (!TargetCharacter) return;
//	if (!InteractableItem) return;
//
//	FGameplayCueParameters Param;
//	Param.SourceObject = InteractableItem; // �ҽ� ������
//	Param.Instigator = TargetCharacter; // ������
//	Param.Location = TargetCharacter->GetActorLocation();
//	InteractableItem->ASC->ExecuteGameplayCue(InteractableItem->GameplayCueTag, Param); // �����ۿ� ������ �����÷��� ť �±� Ȱ��ȭ -> ����
//	UE_LOG(LogTemp, Warning, TEXT("Item Effect Excute"));
//}

