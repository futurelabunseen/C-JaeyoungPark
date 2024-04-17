// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BossAIController.generated.h"

/**
 *
 */
UCLASS()
class PROPJECTPTEST_API ABossAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABossAIController();

	void RunAI();
	void StopAI();

protected:


	virtual void OnPossess(APawn* InPawn) override;

private:

	UPROPERTY()
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY()
	TObjectPtr<class UBehaviorTree> BTAsset;

};
