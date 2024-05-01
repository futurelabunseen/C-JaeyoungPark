// Fill out your copyright notice in the Description page of Project Settings.


// Player Setting Header
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Physics/PPCollision.h"

// GAS Header
#include "Character/PPGASCharacter.h"
#include "AbilitySystemComponent.h"
#include "Player/PPGASPlayerState.h"
#include "EnhancedInputComponent.h"
#include "UI/PPGASWidgetComponent.h"
#include "UI/PPGASUserWidget.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "PropjectPTestGAS.h"
#include "Character/PPComboActionData.h"


APPGASCharacter::APPGASCharacter()
{
	//PPGAS_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("Begin"));
	ASC = nullptr; // 플레이어 스테이트에서 이미 하나 생성했기 때문에 의도적으로 null로 설정
	//PPGAS_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("End"));

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_PPCAPSULE);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Input Action
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionZoomInRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_ZoomIn.IA_ZoomIn'"));
	if (nullptr != InputActionZoomInRef.Object)
	{
		ZoomInAction = InputActionZoomInRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionZoomOutRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_ZoomOut.IA_ZoomOut'"));
	if (nullptr != InputActionZoomOutRef.Object)
	{
		ZoomOutAction = InputActionZoomOutRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Attack.IA_Attack'"));
	if (nullptr != InputActionAttackRef.Object)
	{
		AttackAction = InputActionAttackRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSkillRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Skill.IA_Skill'"));
	if (nullptr != InputActionSkillRef.Object)
	{
		SkillAction = InputActionSkillRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionDashRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Dash.IA_Dash'"));
	if (nullptr != InputActionDashRef.Object)
	{
		DashAction = InputActionDashRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionInteractionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Interaction.IA_Interaction'"));
	if (nullptr != InputActionInteractionRef.Object)
	{
		Interaction = InputActionInteractionRef.Object;
	}

	// ----------------------------------------------------

	static ConstructorHelpers::FObjectFinder<UPPComboActionData> ComboActionDataRef(TEXT("/Script/PropjectPTest.PPComboActionData'/Game/Input/PPA_ComboAttack.PPA_ComboAttack'"));
	if (ComboActionDataRef.Object)
	{
		ComboActionData = ComboActionDataRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage>ComboActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ParagonKwang/Characters/Heroes/Kwang/Animations/AM_ComboAttack.AM_ComboAttack'"));
	if (ComboActionMontageRef.Object)
	{
		ComboActionMontage = ComboActionMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> SKillActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ParagonKwang/Characters/Heroes/Kwang/Animations/AM_SkillAttack.AM_SkillAttack'"));
	if (SKillActionMontageRef.Object)
	{
		SkillActionMontage = SKillActionMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DashActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ParagonKwang/Characters/Heroes/Kwang/Animations/AM_Dash.AM_Dash'"));
	if (DashActionMontageRef.Object)
	{
		DashActionMontage = DashActionMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> InteractionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ParagonKwang/Characters/Heroes/Kwang/Animations/AM_Interaction.AM_Interaction'"));
	if (InteractionMontageRef.Object)
	{
		InteractionMontage = InteractionMontageRef.Object;
	}

	// ----------------------------------------------------

	HpBar = CreateDefaultSubobject<UPPGASWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(200.0f, 20.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

UAbilitySystemComponent* APPGASCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void APPGASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APPGASPlayerState* GASPS = GetPlayerState<APPGASPlayerState>();
	if (GASPS)
	{
		ASC = GASPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(GASPS, this);

		const UPPCharacterAttributeSet* CurrentAttributeSet = ASC->GetSet<UPPCharacterAttributeSet>();
		if (CurrentAttributeSet)
		{
			CurrentAttributeSet->OnOutOfHealth_Player.AddDynamic(this, &ThisClass::OnOutOfHealth);
		}

		for (const auto& StartAbility : StartAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartAbility);
			ASC->GiveAbility(StartSpec);
		}

		for (const auto& StartInputAbility : StartInputAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartInputAbility.Value);
			StartSpec.InputID = StartInputAbility.Key;
			ASC->GiveAbility(StartSpec);
		}

		SetupGASInputComponent(); // PossessedBy 는 서버에서만 호출됨, 클라에서 호출 안 됨

		APlayerController* PlayerContorller = CastChecked<APlayerController>(NewController);
		PlayerContorller->ConsoleCommand(TEXT("showdebug abilitysystem"));
	}
}

void APPGASCharacter::OnRep_PlayerState()
{
	APPGASPlayerState* GASPS = GetPlayerState<APPGASPlayerState>();
	if (GASPS)
	{
		ASC = GASPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(GASPS, this);

		/*const UPPCharacterAttributeSet* CurrentAttributeSet = ASC->GetSet<UPPCharacterAttributeSet>();
		if (CurrentAttributeSet)
		{
			CurrentAttributeSet->OnOutOfHealth_Player.AddDynamic(this, &ThisClass::OnOutOfHealth);
		}*/

		SetupGASInputComponent();
	}
}

void APPGASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Triggered, this, &APPGASCharacter::ZoomIn);
	EnhancedInputComponent->BindAction(ZoomOutAction, ETriggerEvent::Triggered, this, &APPGASCharacter::ZoomOut);

	SetupGASInputComponent();
}

void APPGASCharacter::SetupGASInputComponent()
{
	if (IsValid(ASC) && IsValid(InputComponent))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APPGASCharacter::GASInputPressed, 0);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APPGASCharacter::GASInputReleased, 0);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APPGASCharacter::GASInputPressed, 1);
		EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Triggered, this, &APPGASCharacter::GASInputPressed, 2);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &APPGASCharacter::GASInputPressed, 3);
		EnhancedInputComponent->BindAction(Interaction, ETriggerEvent::Triggered, this, &APPGASCharacter::GASInputPressed, 4);

	}
}

void APPGASCharacter::GASInputPressed(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = true;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			ASC->TryActivateAbility(Spec->Handle);
		}
	}
}

void APPGASCharacter::GASInputReleased(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = false;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
}

void APPGASCharacter::OnOutOfHealth()
{
	SetDead();
}

//Called every frame
void APPGASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//줌 선형보간
	if (FMath::Abs(CameraBoom->TargetArmLength - ExpectedSpringArmLength) > KINDA_SMALL_NUMBER)
	{
		CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, ExpectedSpringArmLength, 0.05f);
	}
}

void APPGASCharacter::ZoomIn()
{
	ExpectedSpringArmLength = FMath::Clamp<float>(ExpectedSpringArmLength - 150.0f, 150.0f, 800.0f);
}

void APPGASCharacter::ZoomOut()
{
	ExpectedSpringArmLength = FMath::Clamp<float>(ExpectedSpringArmLength + 150.0f, 150.0f, 800.0f);
}
