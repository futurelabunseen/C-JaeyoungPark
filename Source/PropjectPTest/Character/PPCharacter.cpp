
#include "Character/PPCharacter.h"
//#include "UObject/ConstructorHelpers.h"
//#include "Camera/CameraComponent.h"
//#include "Components/DecalComponent.h"
//#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
//#include "GameFramework/SpringArmComponent.h"
//#include "Materials/Material.h"
//#include "Engine/World.h"
//#include "Animation/AnimMontage.h"
//#include "InputMappingContext.h"
//#include "EnhancedInputComponent.h"
//#include "EnhancedInputSubsystems.h"
//#include "Physics/PPCollision.h"
//#include "Character/PPComboActionData.h"

APPCharacter::APPCharacter()
{
	//// Don't rotate character to camera direction
	//bUseControllerRotationPitch = false;
	//bUseControllerRotationYaw = false;
	//bUseControllerRotationRoll = false;

	//// Set size for player capsule
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	//GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_PPCAPSULE);

	//// Configure character movement
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	//GetCharacterMovement()->bConstrainToPlane = true;
	//GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonKwang/Characters/Heroes/Kwang/Meshes/Kwang_GDC.Kwang_GDC'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Script/Engine.AnimBlueprint'/Game/ParagonKwang/Characters/Heroes/Kwang/Animations/Kwang_AnimBlueprint.Kwang_AnimBlueprint_C'"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	/*static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonKwang/Characters/Heroes/Kwang/Meshes/Kwang_GDC.Kwang_GDC'"));
	if (nullptr != CharacterMeshRef.Object)
	{
		CharacterMesh = CharacterMeshRef.Object;
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Script/Engine.AnimBlueprint'/Game/ParagonKwang/Characters/Heroes/Kwang/Animations/Kwang_AnimBlueprint.Kwang_AnimBlueprint_C'"));
	if (nullptr != AnimInstanceClassRef.Class)
	{
		AnimInstanceClass = AnimInstanceClassRef.Class;
	}*/

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ParagonKwang/Characters/Heroes/Kwang/Animations/AM_Death.AM_Death'"));
	if (nullptr != DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	//// Create a camera boom...
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	//CameraBoom->TargetArmLength = 800.f;
	//CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	//CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	//// Create a camera...
	//TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	//TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//// Activate ticking in order to update the cursor every frame.
	//PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;

	//// Input
	//static ConstructorHelpers::FObjectFinder<UInputAction> InputActionZoomInRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_ZoomIn.IA_ZoomIn'"));
	//if (nullptr != InputActionZoomInRef.Object)
	//{
	//	ZoomInAction = InputActionZoomInRef.Object;
	//}

	//static ConstructorHelpers::FObjectFinder<UInputAction> InputActionZoomOutRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_ZoomOut.IA_ZoomOut'"));
	//if (nullptr != InputActionZoomOutRef.Object)
	//{
	//	ZoomOutAction = InputActionZoomOutRef.Object;
	//}

	//static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Jump.IA_Jump'"));
	//if (nullptr != InputActionJumpRef.Object)
	//{
	//	JumpAction = InputActionJumpRef.Object;
	//}

	//static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Attack.IA_Attack'"));
	//if (nullptr != InputActionAttackRef.Object)
	//{
	//	AttackAction = InputActionAttackRef.Object;
	//}

	//static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSkillRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Skill.IA_Skill'"));
	//if (nullptr != InputActionSkillRef.Object)
	//{
	//	SkillAction = InputActionSkillRef.Object;
	//}

	//static ConstructorHelpers::FObjectFinder<UInputAction> InputActionDashRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Dash.IA_Dash'"));
	//if (nullptr != InputActionDashRef.Object)
	//{
	//	DashAction = InputActionDashRef.Object;
	//}

	//static ConstructorHelpers::FObjectFinder<UPPComboActionData> ComboActionDataRef(TEXT("/Script/PropjectPTest.PPComboActionData'/Game/Input/PPA_ComboAttack.PPA_ComboAttack'"));
	//if (ComboActionDataRef.Object)
	//{
	//	ComboActionData = ComboActionDataRef.Object;
	//}

	// 어택 몽타주는 각 몬스터에 세팅 -> NPC에 세팅
}

void APPCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APPCharacter::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		DisableInput(PlayerController);
	}
}

void APPCharacter::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage);
}

void APPCharacter::Tick(float DeltaTime)
{

}

////Called every frame
//void APPCharacter::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	//줌 선형보간
//	if (FMath::Abs(CameraBoom->TargetArmLength - ExpectedSpringArmLength) > KINDA_SMALL_NUMBER)
//	{
//		CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, ExpectedSpringArmLength, 0.05f);
//	}
//}
//
//void APPCharacter::ZoomIn()
//{
//	ExpectedSpringArmLength = FMath::Clamp<float>(ExpectedSpringArmLength - 150.0f, 150.0f, 800.0f);
//}
//
//void APPCharacter::ZoomOut()
//{
//	ExpectedSpringArmLength = FMath::Clamp<float>(ExpectedSpringArmLength + 150.0f, 150.0f, 800.0f);
//}
//
//void APPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
//
//	EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Triggered, this, &APPCharacter::ZoomIn);
//	EnhancedInputComponent->BindAction(ZoomOutAction, ETriggerEvent::Triggered, this, &APPCharacter::ZoomOut);
//}