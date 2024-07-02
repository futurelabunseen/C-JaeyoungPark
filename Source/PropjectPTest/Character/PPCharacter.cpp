
#include "Character/PPCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

APPCharacter::APPCharacter()
{
	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
}

void APPCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APPCharacter::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	DeadMulticastRPC();
	SetActorEnableCollision(false);
	IsDeadFlag = true;
}

void APPCharacter::DeadMulticastRPC_Implementation()
{
	if (!HasAuthority())
	{
		PlayDeadAnimation();
	}
}

void APPCharacter::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance))
	{
		AnimInstance->StopAllMontages(0.0f);
		AnimInstance->Montage_Play(DeadMontage);
	}
}