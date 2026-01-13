
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransferVolume.generated.h"

UCLASS()
class PROPJECTPTEST_API ALevelTransferVolume : public AActor
{
	GENERATED_BODY()

public:
	ALevelTransferVolume();

protected:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FString TransferLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Transfer")
	TObjectPtr<class UBoxComponent> TransferVolume;
};
