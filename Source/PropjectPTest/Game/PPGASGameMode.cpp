#include "Game/PPGASGameMode.h"
#include "PropjectPTest.h"
#include "Player/PPGASPlayerState.h"
#include "Character/PPGASCharacter.h"
#include "Player/PPPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Player/PPHUD.h"
#include "Kismet/GameplayStatics.h"


APPGASGameMode::APPGASGameMode()
{
}


void APPGASGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	//PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("============================================================"));
	//PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	//PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("End"));
}

APlayerController* APPGASGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	//PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("Begin"));

	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	//PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("End"));

	return NewPlayerController;
}

void APPGASGameMode::PostLogin(APlayerController* NewPlayer)
{
	//PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::PostLogin(NewPlayer);

	/*UNetDriver* NetDriver = GetNetDriver();
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

	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("End"));*/
}

FTransform APPGASGameMode::GetRandomStartTransform() const // 랜덤한 플레이어 스타트 위치에서 시작
{
	if (PlayerStartArray.Num() == 0)
	{
		return FTransform(FVector(0.0f, 0.0f, 230.0f));
	}

	int32 RandIndex = FMath::RandRange(0, PlayerStartArray.Num() - 1);
	return PlayerStartArray[RandIndex]->GetActorTransform();
}

void APPGASGameMode::StartPlay() // 게임 시작 -> 플레이어 스타트 초기화
{
	Super::StartPlay();

	for (APlayerStart* PlayerStart : TActorRange<APlayerStart>(GetWorld()))
	{
		PlayerStartArray.Add(PlayerStart);
	}

	// 불필요한 Tick 제거
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && !Actor->PrimaryActorTick.bCanEverTick)
		{
			Actor->SetActorTickEnabled(false);
		}
	}

	if (HasAuthority())
	{
		FLatentActionInfo LatentInfo;
		UGameplayStatics::LoadStreamLevel(this, "ElvenRuins_Dungeon", true, true, LatentInfo);
	}
}

void APPGASGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
