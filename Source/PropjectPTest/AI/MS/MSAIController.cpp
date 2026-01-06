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

        // 여기에 RunBehaviorTree 호출 전후 로그 추가
        //UE_LOG(LogTemp, Error, TEXT("%s attempting to RunBehaviorTree for %s"), *GetName(), *GetPawn()->GetName());

        bool RunResult = RunBehaviorTree(BTAsset);
        ensure(RunResult);

        /*if (RunResult)
        {
            UE_LOG(LogTemp, Error, TEXT("%s SUCCESSFULLY ran BehaviorTree for %s"), *GetName(), *GetPawn()->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("%s FAILED to run BehaviorTree for %s"), *GetName(), *GetPawn()->GetName());
        }*/
    }
    /*else
    {
        UE_LOG(LogTemp, Error, TEXT("%s FAILED to use Blackboard for %s"), *GetName(), *GetPawn()->GetName());
    }*/
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
    if (InPawn)
    {
        //UE_LOG(LogTemp, Error, TEXT("AMSAIController::OnPossess called for: %s (ID: %d)"), *InPawn->GetName(), InPawn->GetUniqueID()); // GetUniqueID로 고유 ID 확인
        RunAI();
    }
    /*else
    {
        UE_LOG(LogTemp, Error, TEXT("AMSAIController::OnPossess called with NULL Pawn!"));
    }*/
}
