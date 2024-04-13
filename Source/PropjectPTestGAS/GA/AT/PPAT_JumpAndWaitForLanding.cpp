// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/PPAT_JumpAndWaitForLanding.h"
#include "GameFramework/Character.h"

// ?
UPPAT_JumpAndWaitForLanding::UPPAT_JumpAndWaitForLanding()
{

}

UPPAT_JumpAndWaitForLanding* UPPAT_JumpAndWaitForLanding::CreateTask(UGameplayAbility* OwningAbility)
{
	// 태스크 생성
	UPPAT_JumpAndWaitForLanding* NewTask = NewAbilityTask<UPPAT_JumpAndWaitForLanding>(OwningAbility);
	return NewTask;
}

void UPPAT_JumpAndWaitForLanding::Activate()
{
	Super::Activate();

	ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());

	// 캐릭터 착지 델리게이트 연결
	Character->LandedDelegate.AddDynamic(this, &UPPAT_JumpAndWaitForLanding::OnLandedCallback);

	// 점프
	Character->Jump();

	// ?
	SetWaitingOnAvatar();
}

void UPPAT_JumpAndWaitForLanding::OnDestroy(bool AbilityEnded)
{
	ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());

	// 델리게이트 연결 제거
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
