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

	if (TargetCharacterASC->HasMatchingGameplayTag(PPTAG_CHARACTER_INTERACTIONING)) // 상호작용 어빌리티가 실행되었다면
	{
		// InvokeGameplayCue(TargetCharacter); // 게임플레이 큐 (시각 효과 실행)
		ApplyEffectToTarget(TargetCharacter); // 게임플레이 이펙트 적용

		UWorld* World = GetWorld();
		SpawnParticleEffect(World, TargetCharacter->GetActorLocation(), TargetCharacter->GetActorRotation(), InteractableItem->ParticleSystem);

		// 액터 없애기
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

void UPPGA_Interaction::ApplyEffectToTarget(AActor* Target)
{
	APPGASCharacter* TargetCharacter = Cast<APPGASCharacter>(Target);
	if (TargetCharacterASC && IsValid(TargetCharacter)) // 유효성 검사
	{
		APPGASInteractionItem* InteractableItem = Cast<APPGASInteractionItem>(TargetCharacter->InteractableItem);
		if (IsValid(InteractableItem) && InteractableItem->GameplayEffectClass) // 유효성 검사
		{
			FGameplayEffectContextHandle EffectContext = TargetCharacterASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			FGameplayEffectSpecHandle EffectSpecHandle = TargetCharacterASC->MakeOutgoingSpec(InteractableItem->GameplayEffectClass, 1, EffectContext);
			if (EffectSpecHandle.IsValid()) // 유효성 검사
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
//	Param.SourceObject = InteractableItem; // 소스 아이템
//	Param.Instigator = TargetCharacter; // 가해자
//	Param.Location = TargetCharacter->GetActorLocation();
//	InteractableItem->ASC->ExecuteGameplayCue(InteractableItem->GameplayCueTag, Param); // 아이템에 설정된 게임플레이 큐 태그 활성화 -> 실행
//	UE_LOG(LogTemp, Warning, TEXT("Item Effect Excute"));
//}

