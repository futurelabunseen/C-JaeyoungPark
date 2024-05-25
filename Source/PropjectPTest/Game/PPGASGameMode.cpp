// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PPGASGameMode.h"
#include "PropjectPTest.h"
// #include "PropjectPTest/Player/PPGASPlayerState.h" // �÷��̾� ������Ʈ, ���� ������Ʈ �����ͼ� ����ؾ� ��
#include "Player/PPPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"


APPGASGameMode::APPGASGameMode()
{
	// PlayerStateClass = APPGASPlayerState::StaticClass();
}

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

FTransform APPGASGameMode::GetRandomStartTransform() const // ������ �÷��̾� ��ŸƮ ��ġ���� ����
{
	if (PlayerStartArray.Num() == 0)
	{
		return FTransform(FVector(0.0f, 0.0f, 230.0f));
	}

	int32 RandIndex = FMath::RandRange(0, PlayerStartArray.Num() - 1);
	return PlayerStartArray[RandIndex]->GetActorTransform();
}

void APPGASGameMode::OnPlayerKilled(AController* Killer, AController* KilledPlayer, APawn* KilledPawn) // �÷��̾� ų ī��Ʈ
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

void APPGASGameMode::StartPlay() // ���� ���� -> �÷��̾� ��ŸƮ �ʱ�ȭ
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

	// ���� Ÿ�̸� ����
	// GetWorldTimerManager().SetTimer(GameTimerHandle, this, &APPGASGameMode::DefaultGameTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void APPGASGameMode::DefaultGameTimer() // ���� Ÿ�̸�
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

void APPGASGameMode::FinishMatch() // ���� ���� ����
{
	/*AABGameState* const ABGameState = Cast<AABGameState>(GameState);
	if (ABGameState && IsMatchInProgress())
	{
		EndMatch();
		ABGameState->RemainingTime = ABGameState->ShowResultWaitingTime;
	}*/
}