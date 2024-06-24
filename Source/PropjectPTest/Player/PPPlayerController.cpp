
#include "Player/PPPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/PPCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PropjectPTest.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Player/PPHUD.h"

APPPlayerController::APPPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void APPPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	//if (APPHUD* PlayerHUD = Cast<APPHUD>(GetHUD()))
	//{
	//	// HUD 초기화 확인 로그
	//	UE_LOG(LogTemp, Warning, TEXT("PlayerHUD is valid in APPPlayerController"));
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("PlayerHUD is null in APPPlayerController"));
	//}
}

void APPPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &APPPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &APPPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &APPPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &APPPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &APPPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &APPPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &APPPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &APPPlayerController::OnTouchReleased);
	}
}

void APPPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void APPPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void APPPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn != nullptr)
		{
			// We move there and spawn some particles
			// UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination);
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
			// PP_LOG(LogPPNetwork, Log, TEXT("%s"), *CachedDestination.ToString());
			// PP_LOG(LogPPNetwork, Log, TEXT("%s"), *ControlledPawn->GetActorLocation().ToString());
		}
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void APPPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void APPPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void APPPlayerController::PostNetInit()
{
	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::PostNetInit();

	UNetDriver* NetDriver = GetNetDriver();
	if (NetDriver)
	{
		if (NetDriver->ServerConnection)
		{
			PP_LOG(LogPPNetwork, Log, TEXT("Server Connection: %s"), *NetDriver->ServerConnection->GetName());
		}
	}
	else
	{
		PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("No NetDriver"));
	}

	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("End"));
}

void APPPlayerController::OnPossess(APawn* InPawn)
{
	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::OnPossess(InPawn);

	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("End"));
}

//void APPPlayerController::Tick(float DeltaTime)
//{
//	FHitResult HitResult;
//	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
//
//	// 이전에 활성화된 메쉬 컴포넌트를 추적하여 비활성화합니다.
//	static UMeshComponent* PreviousMeshComp = nullptr;
//	if (PreviousMeshComp && PreviousMeshComp != HitResult.GetComponent())
//	{
//		PreviousMeshComp->SetRenderCustomDepth(false);
//		PreviousMeshComp = nullptr;
//	}
//
//	if (HitResult.bBlockingHit)
//	{
//		UMeshComponent* MeshComp = Cast<UMeshComponent>(HitResult.GetComponent());
//		if (MeshComp)
//		{
//			MeshComp->SetRenderCustomDepth(true);
//			MeshComp->CustomDepthStencilValue = 1;
//			PreviousMeshComp = MeshComp;
//		}
//	}
//}