
#include "Character/PPCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

APPCharacter::APPCharacter()
{
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
}

void APPCharacter::SetDead()
{
	if (!HasAuthority()) return;

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController))
	{
		DisableInput(PlayerController);
		PlayerController->StopMovement();
	}

	SetActorEnableCollision(false);
	IsDeadFlag = true;
	OnRep_IsDeadFlag();
}

void APPCharacter::OnRep_IsDeadFlag()
{
	if (IsDeadFlag)
	{
		PlayDeadAnimation();
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		SetActorEnableCollision(false);
	}
	else
	{
		// 애니메이션 리셋
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance))
		{
			AnimInstance->StopAllMontages(0.2f); // 0.2초 동안 부드럽게 멈춤
		}

		SetActorEnableCollision(true);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

void APPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// IsDeadFlag 변수를 네트워크로 동기화하겠다고 등록
	DOREPLIFETIME(APPCharacter, IsDeadFlag);
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