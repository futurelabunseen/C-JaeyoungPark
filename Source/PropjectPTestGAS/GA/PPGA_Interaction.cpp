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
#include "Net/UnrealNetwork.h"


UPPGA_Interaction::UPPGA_Interaction()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPPGA_Interaction::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(ActorInfo->AvatarActor.Get()); // 현재 플레이어 캐릭터
	APPGASInteractionItem* InteractableItem = Cast<APPGASInteractionItem>(TargetCharacter->InteractableItem); // 현재 상호작용 아이템
	if (!IsValid(TargetCharacter)) // 유효성 검사
	{
		return;
	}

	if (!IsValid(InteractableItem)) // 유효성 검사
	{
		return;
	}

	ActiveInteractionMontage = TargetCharacter->GetInteractionMontage();
	if (!ActiveInteractionMontage) // 유효성 검사
	{
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("InteractionMontage"), ActiveInteractionMontage, 1.0f);
	if (IsValid(PlayMontageTask)) // 유효성 검사
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &UPPGA_Interaction::OnCompleteCallback);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UPPGA_Interaction::OnInterruptedCallback);

		PlayMontageTask->ReadyForActivation();
	}

	TargetCharacterASC = TargetCharacter->GetAbilitySystemComponent(); // 플레이어 ASC
	if (!TargetCharacterASC) // 유효성 검사
	{
		return;
	}

	// PPGAS_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("Begin"));
	if (TargetCharacterASC->HasMatchingGameplayTag(PPTAG_CHARACTER_INTERACTIONING)) // 상호작용 어빌리티가 실행되었다면
	{
		ApplyEffectToTarget(ActorInfo); // 게임플레이 이펙트 적용
		// PPGAS_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("Begin"));
		// ApplyEffectMulicastRPC(ActorInfo);

		// 파티클 생성
		SpawnParticleEffect(GetWorld(), TargetCharacter->GetActorLocation(), TargetCharacter->GetActorRotation(), InteractableItem->ParticleSystem);

		// 액터 없애기
		InteractableItem->Mesh->SetHiddenInGame(true);
		InteractableItem->SetActorEnableCollision(false);
		InteractableItem->SetLifeSpan(2.0f);

	}
}

void UPPGA_Interaction::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	PPGAS_LOG(LogPPGAS, Log, TEXT("Interaction"));
}

void UPPGA_Interaction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(TargetCharacter)) // 유효성 검사
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

void UPPGA_Interaction::ApplyEffectToTarget(const FGameplayAbilityActorInfo* ActorInfo)
{
	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(TargetCharacter)) // 유효성 검사
	{
		PPGAS_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("11111"));
		APPGASInteractionItem* InteractableItem = Cast<APPGASInteractionItem>(TargetCharacter->InteractableItem);
		if (IsValid(InteractableItem)) // 유효성 검사
		{
			PPGAS_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("22222"));
			FGameplayEffectContextHandle EffectContext = TargetCharacterASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			FGameplayEffectSpecHandle EffectSpecHandle = TargetCharacterASC->MakeOutgoingSpec(InteractableItem->GameplayEffectClass, 1, EffectContext);
			if (EffectSpecHandle.IsValid()) // 유효성 검사
			{
				PPGAS_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("33333"));
				TargetCharacterASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
			}
		}
	}
}

void UPPGA_Interaction::SpawnParticleEffect(UWorld* World, FVector Location, FRotator Rotation, UParticleSystem* ParticleSystem)
{
	UGameplayStatics::SpawnEmitterAtLocation(World, ParticleSystem, Location, Rotation, true);
}

//void UPPGA_Interaction::ApplyEffectMulicastRPC_Implementation(const FGameplayAbilityActorInfo* ActorInfo)
//{
//	ApplyEffectToTarget(ActorInfo);
//}
