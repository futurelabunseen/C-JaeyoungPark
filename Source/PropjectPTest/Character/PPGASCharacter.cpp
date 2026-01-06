// Player Setting Header
#include "Character/PPGASCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Physics/PPCollision.h"
#include "Net/UnrealNetwork.h"
#include "PropjectPTest/Game/PPGASGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Monster/MS_Golem.h"
#include "AI/MS/MSAIController.h"
#include "EngineUtils.h"
#include "AI/OctreeSubsystem.h"
#include "UI/PPGASPlayerStatusUserWidget.h"

// GAS Header
#include "AbilitySystemComponent.h"
#include "Player/PPGASPlayerState.h"
#include "EnhancedInputComponent.h"
#include "UI/PPGASWidgetComponent.h"
#include "UI/PPGASUserWidget.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "Character/PPComboActionData.h"
#include "UI/PPGASHpBarUserWidget.h"
#include "Player/PPHUD.h"
#include "Player/PPPlayerController.h"



APPGASCharacter::APPGASCharacter()
{
	ASC = nullptr; // 플레이어 스테이트에서 이미 하나 생성했기 때문에 의도적으로 null로 설정

	Tags.Add(FName("Player"));

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_PPCAPSULE);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> HUDWidgetRef(TEXT("/Game/UI/WBP_PlayerStatus.WBP_PlayerStatus_C"));
	if (HUDWidgetRef.Class)
	{
		HUDWidgetClass = HUDWidgetRef.Class;
	}

	DetectionRadius = 300.0f; // 기본 탐지 반경 설정
}

UAbilitySystemComponent* APPGASCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

// 서버에서 호출
void APPGASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APPGASPlayerState* GASPS = GetPlayerState<APPGASPlayerState>();
	if (IsValid(GASPS))
	{
		ASC = GASPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(GASPS, this);
		// UE_LOG(LogTemp, Error, TEXT("%s Player"), *ASC->GetOwner()->GetName());

		const FGameplayTag PlayerTag = FGameplayTag::RequestGameplayTag(FName("Character.Player"));
		ASC->AddLooseGameplayTag(PlayerTag);


		// PPGAS_LOG(LogPPGASNetwork, Log, TEXT("%s"), TEXT("00000"));
		const UPPCharacterAttributeSet* CurrentAttributeSet = ASC->GetSet<UPPCharacterAttributeSet>();
		if (IsValid(CurrentAttributeSet))
		{
			CurrentAttributeSet->OnOutOfHealth_Player.AddDynamic(this, &ThisClass::OnOutOfHealth);
		}

		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(InitStatEffect, Level, EffectContextHandle);
		if (EffectSpecHandle.IsValid())
		{
			ASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
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
	}
}

void APPGASCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	APPGASPlayerState* GASPS = GetPlayerState<APPGASPlayerState>();
	if (IsValid(GASPS))
	{
		ASC = GASPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(GASPS, this);
		InitializeHUD();
	}
}

void APPGASCharacter::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		UOctreeSubsystem* OctreeSubsystem = World->GetSubsystem<UOctreeSubsystem>();
		if (OctreeSubsystem)
		{
			OctreeSubsystem->RegisterActor(this);
		}
	}
}

void APPGASCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 위젯 제거
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
	}
}

void APPGASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if (IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Triggered, this, &APPGASCharacter::ZoomIn);
		EnhancedInputComponent->BindAction(ZoomOutAction, ETriggerEvent::Triggered, this, &APPGASCharacter::ZoomOut);
	}

	SetupGASInputComponent();
}

void APPGASCharacter::SetupGASInputComponent()
{
	//if (IsValid(ASC) && IsValid(InputComponent))
	//{
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if (IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APPGASCharacter::GASInputPressed, 0);
		EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Triggered, this, &APPGASCharacter::GASInputPressed, 1);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &APPGASCharacter::GASInputPressed, 2);
		EnhancedInputComponent->BindAction(Interaction, ETriggerEvent::Triggered, this, &APPGASCharacter::GASInputPressed, 3);
	}
	//}
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

	if (HUDWidget)
	{
		// 화면에서 HUD를 숨깁니다.
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	GetWorldTimerManager().SetTimer(DeadTimerHandle, this, &APPGASCharacter::ResetPlayer, 3.0f, false);
}

//Called every frame
void APPGASCharacter::Tick(float DeltaTime)
{
	//줌 선형보간
	if (FMath::Abs(CameraBoom->TargetArmLength - ExpectedSpringArmLength) > KINDA_SMALL_NUMBER)
	{
		CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, ExpectedSpringArmLength, 0.05f);
	}
}

void APPGASCharacter::ZoomIn()
{
	ExpectedSpringArmLength = FMath::Clamp<float>(ExpectedSpringArmLength - ZoomMinLength, ZoomMinLength, ZoomMaxLength);
}

void APPGASCharacter::ZoomOut()
{
	ExpectedSpringArmLength = FMath::Clamp<float>(ExpectedSpringArmLength + ZoomMinLength, ZoomMinLength, ZoomMaxLength);
}

void APPGASCharacter::ResetPlayer() // 플레이어 리셋(리스폰)
{
	// SetupGASInputComponent();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	SetActorEnableCollision(true);

	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(InitStatEffect, Level, EffectContextHandle);
	if (EffectSpecHandle.IsValid())
	{
		ASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
		// UE_LOG(LogTemp, Error, EffectSpecHandle.Data->get);
	}

	APPGASGameMode* PPGASGameMode = GetWorld()->GetAuthGameMode<APPGASGameMode>();
	if (IsValid(PPGASGameMode))
	{
		FTransform NewTransform = PPGASGameMode->GetRandomStartTransform();
		TeleportTo(NewTransform.GetLocation(), NewTransform.GetRotation().Rotator());
	}

	IsDeadFlag = false;

	if (HUDWidget)
	{
		HUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

// [추가] UI 생성 및 초기화 전용 함수
void APPGASCharacter::InitializeHUD()
{
	// 1. 내 캐릭터가 로컬 플레이어(내 화면)인지 확인합니다. (AI나 다른 플레이어 머리 위에 뜨면 안 되니까!)
	if (IsLocallyControlled() && IsValid(HUDWidgetClass))
	{
		// 2. 이미 생성된 게 없다면 새로 만듭니다.
		if (!IsValid(HUDWidget))
		{
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC)
			{
				HUDWidget = CreateWidget<UPPGASPlayerStatusUserWidget>(PC, HUDWidgetClass);

				if (HUDWidget)
				{
					// 3. 화면(Viewport)에 띄웁니다.
					HUDWidget->AddToViewport();

					// 4. ASC를 연결해서 체력/마나 업데이트를 시작합니다.
					HUDWidget->SetAbilitySystemComponent(ASC->GetOwner());
				}
			}
		}
	}
}

void APPGASCharacter::LevelUp()
{
	// 1. 레벨 증가
	Level++;
	UE_LOG(LogTemp, Warning, TEXT("Level Up! New Level: %d"), Level);

	// 2. 최대 스탯(Max) 증가 적용
	if (IsValid(ASC) && IsValid(InitStatEffect))
	{
		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		// 변경된 Level 변수를 넣어주어 MaxHealth, MaxMana 등을 늘려줍니다.
		FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(InitStatEffect, Level, EffectContextHandle);

		if (EffectSpecHandle.IsValid())
		{
			ASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
		}
	}

	// 3. [추가] 체력/마나 완전 회복 (Max가 늘어난 상태에서 채워줍니다)
	if (IsValid(ASC) && IsValid(LevelUpHealEffect))
	{
		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);

		// 레벨은 상관없으므로 1.0f로 적용
		FGameplayEffectSpecHandle HealSpecHandle = ASC->MakeOutgoingSpec(LevelUpHealEffect, 1.0f, EffectContextHandle);

		if (HealSpecHandle.IsValid())
		{
			ASC->BP_ApplyGameplayEffectSpecToSelf(HealSpecHandle);
			UE_LOG(LogTemp, Log, TEXT("Level Up Heal Applied!"));
		}
	}
}

// [추가] 변수 리플리케이션 등록
void APPGASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APPGASCharacter, Level);
}