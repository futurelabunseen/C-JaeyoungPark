// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PPCharacterNonPlayer.h"

APPCharacterNonPlayer::APPCharacterNonPlayer()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
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
