// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PPCharacterNonPlayer.h"
#include "Engine/AssetManager.h"

APPCharacterNonPlayer::APPCharacterNonPlayer()
{
	GetMesh()->SetHiddenInGame(true);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void APPCharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ensure(NPCMeshes.Num() > 0);
	int32 RandIndex = FMath::RandRange(0, NPCMeshes.Num() - 1);
	NPCMeshHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(NPCMeshes[RandIndex], FStreamableDelegate::CreateUObject(this, &APPCharacterNonPlayer::NPCMeshLoadCompleted));
}

void APPCharacterNonPlayer::SetDead()
{
	Super::SetDead();

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	), DeadEventDelayTime, false);
}

void APPCharacterNonPlayer::NPCMeshLoadCompleted()
{
	if (NPCMeshHandle.IsValid())
	{
		USkeletalMesh* NPCMesh = Cast<USkeletalMesh>(NPCMeshHandle->GetLoadedAsset());
		if (NPCMesh)
		{
			GetMesh()->SetSkeletalMesh(NPCMesh);
			GetMesh()->SetHiddenInGame(false);
		}
	}

	NPCMeshHandle->ReleaseHandle();
}

//void APPCharacterNonPlayer::MonsterAttack()
//{
//
//}

//float APPCharacterNonPlayer::GetAIPatrolRadius()
//{
//	return 800.0f;
//}
//
//float APPCharacterNonPlayer::GetAIDetectRange()
//{
//	return 1000.0f;
//}
//
//float APPCharacterNonPlayer::GetAIAttackRange()
//{
//	return AttributeSet->GetAttackRange();
//}
//
//float APPCharacterNonPlayer::GetAITurnSpeed()
//{
//	return 2.0f;
//}
//
//void APPCharacterNonPlayer::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
//{
//	OnAttackFinished = InOnAttackFinished;
//}
//
//void APPCharacterNonPlayer::AttackByAI()
//{
//	// ������ ������ �ٸ�
//	// ĳ���� ���̽����� ���� ���� �� ����
//}
//
//void APPCharacterNonPlayer::NotifyComboActionEnd()
//{
//	Super::NotifyComboActionEnd();
//	OnAttackFinished.ExecuteIfBound();
//}
