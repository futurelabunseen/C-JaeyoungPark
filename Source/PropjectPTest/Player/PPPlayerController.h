
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PPPlayerController.generated.h"

UCLASS()
class PROPJECTPTEST_API APPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APPPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UNiagaraSystem> FXCursor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SetDestinationClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SetDestinationTouchAction;

protected:
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;
	virtual void BeginPlay();
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();
	virtual void PostNetInit() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	FVector CachedDestination;

	bool bIsTouch;
	float FollowTime;
};


