#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Detect_Octree.generated.h"

class UOctreeSubsystem;

UCLASS()
class PROPJECTPTEST_API UBTService_Detect_Octree : public UBTService
{
    GENERATED_BODY()
public:
    UBTService_Detect_Octree();
protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
private:
    // UOctreeSubsystem 포인터를 캐싱합니다.
    // Transient 속성은 레벨 이동 시 자동으로 null로 초기화되어 안전합니다.
    UPROPERTY(Transient)
    TObjectPtr<UOctreeSubsystem> CachedOctreeSubsystem; // ◀◀◀ 이전에 추가했던 부분입니다.
};