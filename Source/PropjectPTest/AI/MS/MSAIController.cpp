// AMSAIController.cpp

#include "AI/MS/MSAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/MS/MSAI.h"

AMSAIController::AMSAIController()
{
    static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(TEXT("/Game/AI/BB_NormalMonster.BB_NormalMonster"));
    if (BBAssetRef.Succeeded())
    {
        BBAsset = BBAssetRef.Object;
    }

    static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(TEXT("/Game/AI/BT_NormalMonster.BT_NormalMonster"));
    if (BTAssetRef.Succeeded())
    {
        BTAsset = BTAssetRef.Object;
    }
}

void AMSAIController::RunAI()
{
    UBlackboardComponent* BlackboardPtr = Blackboard.Get();

    if (UseBlackboard(BBAsset, BlackboardPtr))
    {
        Blackboard->SetValueAsVector(FName("HomePos"), GetPawn()->GetActorLocation());

        bool RunResult = RunBehaviorTree(BTAsset);
        ensure(RunResult);
    }
}

void AMSAIController::StopAI()
{
    UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
    if (BTComponent)
    {
        BTComponent->StopTree(EBTStopMode::Safe);
    }
}

void AMSAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    RunAI();
}

//// Behavior Tree 활성화 상태 확인 메서드 추가
//bool AMSAIController::IsBehaviorTreeActive() const
//{
//    UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
//    return BTComponent && BTComponent->IsRunning();
//}
