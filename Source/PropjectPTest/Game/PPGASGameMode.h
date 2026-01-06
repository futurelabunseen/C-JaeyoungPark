
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PPGASGameMode.generated.h"

UCLASS()
class PROPJECTPTEST_API APPGASGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	APPGASGameMode();

	virtual FTransform GetRandomStartTransform() const;

protected:
	virtual void PostInitializeComponents() override;
	virtual void StartPlay() override;

	FTimerHandle GameTimerHandle;
	TArray<TObjectPtr<class APlayerStart>> PlayerStartArray;

private:
	// Network Check Session
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
