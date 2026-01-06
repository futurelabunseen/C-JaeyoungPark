
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BossAIInterface.generated.h"

UINTERFACE(MinimalAPI)
class UBossAIInterface : public UInterface
{
	GENERATED_BODY()
};

class PROPJECTPTEST_API IBossAIInterface
{
	GENERATED_BODY()

public:
	virtual float GetAIPatrolRadius() = 0;
	virtual float GetAIDetectRange() = 0;
	virtual float GetAIAttackRange() = 0;
	virtual float GetAITurnSpeed() = 0;
};
