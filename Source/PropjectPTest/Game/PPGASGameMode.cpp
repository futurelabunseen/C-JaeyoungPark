// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PPGASGameMode.h"
#include "PropjectPTest.h"
#include "Player/PPGASPlayerState.h" // 플레이어 스테이트, 게임 스테이트 가져와서 사용해야 함
#include "Character/PPGASCharacter.h"
#include "Player/PPPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Player/PPHUD.h"
#include "Kismet/GameplayStatics.h"


APPGASGameMode::APPGASGameMode()
{
	// DefaultPawnClass = APPGASCharacter::StaticClass();
	// PlayerControllerClass = APPPlayerController::StaticClass();
	// HUDClass = APPHUD::StaticClass();
}

//void APPGASGameMode::BeginPlay()
//{
//	Super::BeginPlay();
//
//	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
//	if (PlayerController)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("PlayerController is valid in GameMode BeginPlay"));
//
//		APPHUD* PlayerHUD = Cast<APPHUD>(PlayerController->GetHUD());
//		if (PlayerHUD)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("PlayerHUD is valid in GameMode BeginPlay"));
//		}
//		else
//		{
//			UE_LOG(LogTemp, Error, TEXT("PlayerHUD is null in GameMode BeginPlay"));
//		}
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("PlayerController is null in GameMode BeginPlay"));
//	}
//}


void APPGASGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("============================================================"));
	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("End"));
}

APlayerController* APPGASGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("Begin"));

	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("End"));

	return NewPlayerController;
}

void APPGASGameMode::PostLogin(APlayerController* NewPlayer)
{
	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::PostLogin(NewPlayer);

	UNetDriver* NetDriver = GetNetDriver();
	if (NetDriver)
	{
		if (NetDriver->ClientConnections.Num() == 0)
		{
			PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("No Client Connection"));
		}
		else
		{
			for (const auto& Connection : NetDriver->ClientConnections)
			{
				PP_LOG(LogPPNetwork, Log, TEXT("Client Connections: %s"), *Connection->GetName());
			}
		}
	}
	else
	{
		PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("No NetDriver"));
	}

	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("End"));
}

FTransform APPGASGameMode::GetRandomStartTransform() const // 랜덤한 플레이어 스타트 위치에서 시작
{
	if (PlayerStartArray.Num() == 0)
	{
		return FTransform(FVector(0.0f, 0.0f, 230.0f));
	}
	
	/*for (const auto& PlayerStarts : PlayerStartArray)
	{
		if (PlayerStarts->PlayerStartTag == FName("Dungeon"))
		{
			int32 RandIndex = FMath::RandRange(0, PlayerStartArray.Num() - 1);
			return PlayerStartArray[RandIndex]->GetActorTransform();
		}
	}
	return FTransform(FVector(0.0f, 0.0f, 230.0f));*/

	int32 RandIndex = FMath::RandRange(0, PlayerStartArray.Num() - 1);
	return PlayerStartArray[RandIndex]->GetActorTransform();
}

void APPGASGameMode::OnPlayerKilled(AController* Killer, AController* KilledPlayer, APawn* KilledPawn) // 플레이어 킬 카운트
{
	/*PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("Begin"));

	APlayerState* KillerPlayerState = Killer->PlayerState;
	if (KillerPlayerState)
	{
		KillerPlayerState->SetScore(KillerPlayerState->GetScore() + 1);

		if (KillerPlayerState->GetScore() > 2)
		{
			FinishMatch();
		}
	}*/
}

void APPGASGameMode::StartPlay() // 게임 시작 -> 플레이어 스타트 초기화
{
	Super::StartPlay();

	for (APlayerStart* PlayerStart : TActorRange<APlayerStart>(GetWorld()))
	{
		PlayerStartArray.Add(PlayerStart);
	}
}

void APPGASGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 월드 타이머 설정
	// GetWorldTimerManager().SetTimer(GameTimerHandle, this, &APPGASGameMode::DefaultGameTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void APPGASGameMode::DefaultGameTimer() // 게임 타이머
{
	/*AABGameState* const APPGASGameMode = Cast<AABGameState>(GameState);

	if (ABGameState && ABGameState->RemainingTime > 0)
	{
		ABGameState->RemainingTime--;
		PP_LOG(LogPPNetwork, Log, TEXT("Remaining Time : %d"), ABGameState->RemainingTime);
		if (ABGameState->RemainingTime <= 0)
		{
			if (GetMatchState() == MatchState::InProgress)
			{
				FinishMatch();
			}
			else if (GetMatchState() == MatchState::WaitingPostMatch)
			{
				GetWorld()->ServerTravel(TEXT("/Game/ArenaBattle/Maps/Part3Step2?listen"));
			}
		}
	}*/
}

void APPGASGameMode::FinishMatch() // 게임 종료 실행
{
	/*AABGameState* const ABGameState = Cast<AABGameState>(GameState);
	if (ABGameState && IsMatchInProgress())
	{
		EndMatch();
		ABGameState->RemainingTime = ABGameState->ShowResultWaitingTime;
	}*/
}

// 첫 실행 시 스폰될 플레이어 스타트 지점 오버라이딩
//AActor* APPGASGameMode::ChoosePlayerStart_Implementation(AController* Player)
//{
//	//기존의 플레이어 스타트 액터 리턴값 변경
//	TArray<class AActor*> OutActors;
//	TArray<class APlayerStart*> TargetPlayerStarts;
//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), playerStartFactory, OutActors);
//	for (const auto PlayerStarts : OutActors)
//	{
//		if (const auto PlayerStart = Cast<APlayerStart>(PlayerStarts))
//		{
//			if (PlayerStart && PlayerStart->PlayerStartTag == FName("Hideout"))
//			{
//				const FVector Center = PlayerStart->GetActorLocation();
//				FCollisionQueryParams Params;
//				bool AlreadyLocated = false;
//				if (TArray<FOverlapResult> HitObj; GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(50), Params))
//				{
//					for (int i = 0; i < HitObj.Num(); ++i)
//					{
//						if (Cast<APlayerCharacter>(HitObj[i].GetActor()))
//						{
//							AlreadyLocated = true;
//							break;
//						}
//					}
//				}
//				if (AlreadyLocated)
//				{
//					continue;
//				}
//				TargetPlayerStarts.Add(PlayerStart);
//			}
//		}
//	}
//	if (const auto PlayerStartRandIndex = FMath::RandRange(0, TargetPlayerStarts.Num() - 1); TargetPlayerStarts.IsValidIndex(PlayerStartRandIndex))
//	{
//		APlayerStart* const TargetPlayerStart = TargetPlayerStarts[PlayerStartRandIndex];
//		return TargetPlayerStart;
//	}
//	return nullptr;
//}
