// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PPGASGameMode.h"
#include "PropjectPTest.h"
// #include "PropjectPTestGAS/Player/PPGASPlayerState.h"


APPGASGameMode::APPGASGameMode()
{
	//static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/Blueprint/BP_PPGASCharacter.BP_PPGASCharacter_C'"));
	//if (DefaultPawnClassRef.Class)
	//{
	//	DefaultPawnClass = DefaultPawnClassRef.Class;
	//}

	//static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/CoreUObject.Class'/Script/PropjectPTest.PPPlayerController'"));

	//if (PlayerControllerClassRef.Class)
	//{
	//	// DefaultPawnClass
	//	PlayerControllerClass = PlayerControllerClassRef.Class;
	//}

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

void APPGASGameMode::StartPlay()
{
	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("Begin"));

	Super::StartPlay();

	PP_LOG(LogPPNetwork, Log, TEXT("%s"), TEXT("End"));
}
