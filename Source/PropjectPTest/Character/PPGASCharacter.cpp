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
	// PPGAS_LOG(LogPPGASNetwork, Log, TEXT("%s"), TEXT("End"));

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
		HpBar->InitWidget();
	}

	// 어빌리티 시스템 디버그 용
	/*if (IsLocallyControlled() && Controller)
	{
		APlayerController* PlayerContorller = CastChecked<APlayerController>(GetController());
		PlayerContorller->ConsoleCommand(TEXT("showdebug abilitysystem"));
	}*/
}

void APPGASCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 전체 액터 탐색이 아닌, 범위 탐색을 위한 타이머 설정
	// GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &APPGASCharacter::DetectMonstersInRadius, 1.0f, true, 1.0f);
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
	HpBar->SetHiddenInGame(true);
	SetDead();
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
}

//void APPGASCharacter::DetectMonstersInRadius()
//{
//	// 범위 내 AI 몬스터 탐지 및 BT 실행
//	TArray<AActor*> OverlappingActors;
//	GetOverlappingActors(OverlappingActors, AMS_Golem::StaticClass());
//
//	for (AActor* Actor : OverlappingActors)
//	{
//		if (FVector::Dist(Actor->GetActorLocation(), GetActorLocation()) <= DetectionRadius)
//		{
//			AMS_Golem* MS_Golem = Cast<AMS_Golem>(Actor);
//			if (MS_Golem)
//			{
//				AMSAIController* AIController = Cast<AMSAIController>(MS_Golem->GetController());
//				if (AIController && !AIController->IsBehaviorTreeActive())
//				{
//					AIController->RunAI();
//				}
//			}
//		}
//	}
//
//	// 범위 밖 AI 몬스터 탐지 및 BT 중지
//	for (TActorIterator<AMS_Golem> It(GetWorld()); It; ++It)
//	{
//		AMS_Golem* MS_Golem = *It;
//		if (MS_Golem)
//		{
//			AMSAIController* AIController = Cast<AMSAIController>(MS_Golem->GetController());
//			if (AIController && AIController->IsBehaviorTreeActive())
//			{
//				if (FVector::Dist(MS_Golem->GetActorLocation(), GetActorLocation()) > DetectionRadius)
//				{
//					AIController->StopAI();
//				}
//			}
//		}
//	}
//}


//void APPGASCharacter::MoveToStreamingLevel(const bool IsPlayerDeath)
//{
//	if (IsLocallyControlled())
//	{
//		const FName DungeonLevelName = FName("ElvenRuins");
//		const FName BossLevelName = FName("_mansion_");
//		const FName OnLevelLoadFinishedFunc = FName("OnStreamingLevelLoadFinished");
//		FLatentActionInfo LoadLatentInfo;
//		LoadLatentInfo.CallbackTarget = this;
//		LoadLatentInfo.Linkage = 0;
//		LoadLatentInfo.ExecutionFunction = OnLevelLoadFinishedFunc;
//
//		UnloadMultipleStreamingLevels(DungeonLevelName); // 던전 레벨 언로드
//		UGameplayStatics::LoadStreamLevel(this, BossLevelName, true, true, LoadLatentInfo); // 보스 레벨 로드
//	}
//}

//void APPGASCharacter::UnloadMultipleStreamingLevels(const FName& LevelName)
//{
//	if (LevelName != NAME_None)
//	{
//		UGameplayStatics::UnloadStreamLevel(this, LevelName, FLatentActionInfo(), false);
//	}
//}