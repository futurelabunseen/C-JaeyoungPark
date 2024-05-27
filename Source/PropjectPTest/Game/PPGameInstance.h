
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h" // EOnJoinSessionCompleteResult 정의 포함
#include "PPGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString SessionName;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPlayers;

	UPROPERTY(BlueprintReadOnly)
	bool IsLan;

	UPROPERTY(BlueprintReadOnly)
	int32 Ping;

	UPROPERTY(BlueprintReadOnly)
	int32 ServerArrayIndex;

	void SetPlayerCount()
	{
		// Implementation for setting player count
	}
};

USTRUCT(BlueprintType)
struct FCreateSessionInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FString RoomName;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxPlayers;

	UPROPERTY(BlueprintReadWrite)
	bool IsLan;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSearchingServer, bool, IsSearching);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServerList, const FServerInfo&, ServerInfo);

UCLASS()
class PROPJECTPTEST_API UPPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPPGameInstance();

	virtual void Init() override;

	// UPPGameInstance 클래스에 함수 선언 추가
	UFUNCTION(BlueprintCallable)
	void CreateServer(const FCreateSessionInfo& CreateSessionInfo);

	UFUNCTION(BlueprintCallable)
	void FindSession();

	UFUNCTION(BlueprintCallable)
	void JoinServer(int32 ArrayIndex);

	UFUNCTION(BlueprintCallable)
	void DestroySession();

	UPROPERTY(BlueprintAssignable)
	FOnSearchingServer OnSearchingServer;

	UPROPERTY(BlueprintAssignable)
	FOnServerList OnServerList;

private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> SessionSettings;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	// Delegate handlers
	void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	void OnFindSessionsComplete(bool Succeeded);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool Succeeded);

	bool bCreateSessionOnDestroy = false;
	FString MenuLevelPath = "/Game/Maps/Demonstration_Village.Demonstration_Village"; // Example path, replace with actual menu level path
	FName CurrentSessionName = NAME_GameSession;
};
