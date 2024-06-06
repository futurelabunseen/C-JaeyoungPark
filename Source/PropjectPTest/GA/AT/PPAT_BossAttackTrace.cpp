// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/PPAT_BossAttackTrace.h"
#include "GA/TA/PPTA_BossAttackTrace.h"
#include "AbilitySystemComponent.h"

UPPAT_BossAttackTrace::UPPAT_BossAttackTrace()
{
}

UPPAT_BossAttackTrace* UPPAT_BossAttackTrace::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<APPTA_BossAttackTrace> TargetActorClass)
{
	UPPAT_BossAttackTrace* NewTask = NewAbilityTask<UPPAT_BossAttackTrace>(OwningAbility);
	NewTask->TargetActorClass = TargetActorClass;
	return NewTask;
}

void UPPAT_BossAttackTrace::Activate()
{
	Super::Activate();

	SpawnAndInitializeTargetActor();
	FinalizeTargetActor();

	SetWaitingOnAvatar();
}

void UPPAT_BossAttackTrace::OnDestroy(bool AbilityEnded)
{
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->Destroy();
	}

	Super::OnDestroy(AbilityEnded);
}

void UPPAT_BossAttackTrace::SpawnAndInitializeTargetActor()
{
	SpawnedTargetActor = Cast<APPTA_BossAttackTrace>(Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->SetShowDebug(true);
		SpawnedTargetActor->TargetDataReadyDelegate.AddUObject(this, &UPPAT_BossAttackTrace::OnTargetDataReadyCallback);
	}
}

void UPPAT_BossAttackTrace::FinalizeTargetActor()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC)
	{
		const FTransform SpawnTransform = ASC->GetAvatarActor()->GetTransform();
		SpawnedTargetActor->FinishSpawning(SpawnTransform);

		ASC->SpawnedTargetActors.Push(SpawnedTargetActor);
		SpawnedTargetActor->StartTargeting(Ability);
		SpawnedTargetActor->ConfirmTargeting();
	}
}

void UPPAT_BossAttackTrace::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast(DataHandle);
	}

	EndTask();
}
