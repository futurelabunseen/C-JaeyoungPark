
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MSAIController.generated.h"

UCLASS()
class PROPJECTPTEST_API AMSAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMSAIController();

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
