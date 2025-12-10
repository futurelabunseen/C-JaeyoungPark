#pragma once

#include "CoreMinimal.h"
#include "Chaos/CacheManagerActor.h" // 부모 클래스 변경
#include "ChaosRock.generated.h"

class UGeometryCollectionComponent;
class UBoxComponent;
class UStaticMeshComponent;
class UAudioComponent;
class USoundBase;

/**
 * AChaosRock
 * 스스로 캐시 매니저 역할을 하며, 플레이어 접촉 시 캐시된 파괴 효과를 재생하는 액터
 */
UCLASS()
class PROPJECTPTEST_API AChaosRock : public AChaosCacheManager
{
    GENERATED_BODY()

public:
    AChaosRock();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void OnConstruction(const FTransform& Transform) override;

public:
    // 트리거 감지 (박스 콜리전)
    UFUNCTION()
    void NotifyActorBeginOverlap(AActor* Other) override;

    // 파괴 실행 함수
    UFUNCTION(BlueprintCallable, Category = "Destruction")
    void TriggerDestruction();

    //virtual void Tick(float DeltaTime) override;

protected:
    // 1. 카오스 메시 (파괴될 지오메트리 컬렉션)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destruction")
    UGeometryCollectionComponent* RockMesh;

    // 2. 스태틱 메시 (파괴 전 깔끔한 외형 + 최적화용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destruction")
    UStaticMeshComponent* RockStaticMesh;

    // 3. 트리거 박스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destruction")
    UBoxComponent* BoxCollision;

    // 4. 사운드 컴포넌트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    UAudioComponent* DestructionSound;

    // 파괴 시 재생할 사운드 큐
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* DestructionSoundCue;

private:
    // 이미 파괴되었는지 체크하는 플래그
    bool bHasTriggered;

    // 사운드 페이드 아웃 시간
    float FadeOutDuration;

    float RunningTime;
};