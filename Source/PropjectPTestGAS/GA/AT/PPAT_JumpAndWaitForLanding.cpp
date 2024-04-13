// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/PPAT_JumpAndWaitForLanding.h"
#include "GameFramework/Character.h"

// ?
UPPAT_JumpAndWaitForLanding::UPPAT_JumpAndWaitForLanding()
{

}

UPPAT_JumpAndWaitForLanding* UPPAT_JumpAndWaitForLanding::CreateTask(UGameplayAbility* OwningAbility)
{
	// �½�ũ ����
	UPPAT_JumpAndWaitForLanding* NewTask = NewAbilityTask<UPPAT_JumpAndWaitForLanding>(OwningAbility);
	return NewTask;
}

void UPPAT_JumpAndWaitForLanding::Activate()
{
	Super::Activate();

	ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());

	// ĳ���� ���� ��������Ʈ ����
	Character->LandedDelegate.AddDynamic(this, &UPPAT_JumpAndWaitForLanding::OnLandedCallback);

	// ����
	Character->Jump();

	// ?
	SetWaitingOnAvatar();
}

void UPPAT_JumpAndWaitForLanding::OnDestroy(bool AbilityEnded)
{
	ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());

	// ��������Ʈ ���� ����
	Character->LandedDelegate.RemoveDynamic(this, &UPPAT_JumpAndWaitForLanding::OnLandedCallback);


	Super::OnDestroy(AbilityEnded);
}

void UPPAT_JumpAndWaitForLanding::OnLandedCallback(const FHitResult& Hit)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast();
	}
}
