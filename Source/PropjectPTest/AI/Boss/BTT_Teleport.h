#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Teleport.generated.h"

UCLASS()
class PROPJECTPTEST_API UBTT_Teleport : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    struct FBlackboardKeySelector TargetPosKey;

protected:
    EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};