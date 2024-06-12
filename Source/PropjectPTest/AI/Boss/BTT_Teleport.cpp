
#include "BTT_Teleport.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

EBTNodeResult::Type UBTT_Teleport::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* AIPawn = AIController->GetPawn();
    if (AIPawn == nullptr) return EBTNodeResult::Failed;

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    FVector TargetLocation = BlackboardComp->GetValueAsVector(TargetPosKey.SelectedKeyName);
    AIPawn->SetActorLocation(TargetLocation);

    return EBTNodeResult::Succeeded;
}