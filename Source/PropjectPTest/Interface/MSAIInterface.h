
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MSAIInterface.generated.h"

UINTERFACE(MinimalAPI)
class UMSAIInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DELEGATE(FAICharacterAttackFinished);

class PROPJECTPTEST_API IMSAIInterface
{
	GENERATED_BODY()

public:
	virtual float GetAIPatrolRadius() = 0;
	virtual float GetAIDetectRange() = 0;
	virtual float GetAIAttackRange() = 0;
	virtual float GetAITurnSpeed() = 0;
};
