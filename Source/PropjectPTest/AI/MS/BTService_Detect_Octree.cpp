#include "AI/MS/BTService_Detect_Octree.h"
#include "MSAI.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/MSAIInterface.h"
#include "Kismet/GameplayStatics.h"
//#include "AI/OctreeManager.h"
#include "AI/OctreeSubsystem.h"
#include "DrawDebugHelpers.h"

UBTService_Detect_Octree::UBTService_Detect_Octree()
{
    NodeName = TEXT("Detect (Octree)");
    Interval = 1.0f;
    RandomDeviation = 0.2f; // 여러 AI의 탐색 주기를 분산시켜 성능 스파이크를 완화
}

void UBTService_Detect_Octree::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (nullptr == ControllingPawn) return;

    UWorld* World = ControllingPawn->GetWorld();
    if (nullptr == World) return;

    if (!IsValid(CachedOctreeSubsystem))
    {
        CachedOctreeSubsystem = World->GetSubsystem<UOctreeSubsystem>();
    }
    UOctreeSubsystem* OctreeSubsystem = CachedOctreeSubsystem; // 캐시된 포인터 사용
    if (nullptr == OctreeSubsystem) return;

    /*AOctreeManager* OctreeManager = Cast<AOctreeManager>(UGameplayStatics::GetActorOfClass(World, AOctreeManager::StaticClass()));
    if (nullptr == OctreeManager) return;*/

    IMSAIInterface* AIPawn = Cast<IMSAIInterface>(ControllingPawn);
    if (nullptr == AIPawn) return;

    const FVector Center = ControllingPawn->GetActorLocation();
    const float DetectRadius = AIPawn->GetAIDetectRange();

    TArray<AActor*> NearbyActors;
    OctreeSubsystem->FindNearbyActors(Center, DetectRadius, NearbyActors);
    //OctreeManager->FindNearbyActors(Center, DetectRadius, NearbyActors);

    APawn* ClosestPlayerPawn = nullptr;
    float MinDistanceSq = FMath::Square(DetectRadius);

    for (auto const& NearbyActor : NearbyActors)
    {
        if (NearbyActor == nullptr) continue;

        // 1. 이 액터가 Pawn 클래스가 맞는지 확인
        APawn* Pawn = Cast<APawn>(NearbyActor);
        if (Pawn == nullptr) continue;

        // 2. Pawn에 컨트롤러가 붙어있는지 확인
        AController* PawnController = Pawn->GetController();
        if (PawnController == nullptr) continue;

        // 3. 그 컨트롤러가 플레이어 컨트롤러인지 확인
        if (PawnController->IsPlayerController())
        {
            float DistanceSq = FVector::DistSquared(Center, Pawn->GetActorLocation());
            if (DistanceSq < MinDistanceSq)
            {
                MinDistanceSq = DistanceSq;
                ClosestPlayerPawn = Pawn;
            }
        }
    }

    if (ClosestPlayerPawn)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, ClosestPlayerPawn);
        DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
        DrawDebugLine(World, ControllingPawn->GetActorLocation(), ClosestPlayerPawn->GetActorLocation(), FColor::Green, false, 0.27f);
    }
    else
    {
        if (OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET) != nullptr)
        {
            OwnerComp.GetBlackboardComponent()->ClearValue(BBKEY_TARGET);
        }
        DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
    }
}