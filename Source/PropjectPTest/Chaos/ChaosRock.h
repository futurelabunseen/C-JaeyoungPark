#pragma once

#include "Chaos/CacheManagerActor.h"
#include "CoreMinimal.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "ChaosRock.generated.h"

class UGeometryCollectionComponent;
class UBoxComponent;
class UStaticMeshComponent;
class UAudioComponent;
class USoundBase;


UCLASS()
class PROPJECTPTEST_API AChaosRock : public AChaosCacheManager
{
    GENERATED_BODY()

public:
    AChaosRock();

protected:
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

public:
    // 트리거 감지
    UFUNCTION()
    virtual void NotifyActorBeginOverlap(AActor* Other) override;

    // 파괴 실행 함수
    UFUNCTION(BlueprintCallable, Category = "Destruction")
    void TriggerDestruction();

protected:
    // 1. 카오스 메시 (파괴될 지오메트리 컬렉션)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destruction")
    UGeometryCollectionComponent* RockMesh;

    // 2. 스태틱 메시 (파괴 전 외형)
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

    // ★ [추가] 재생할 레벨 시퀀스 에셋 (에디터에서 할당)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destruction|Sequence")
    ULevelSequence* DestructionSequence;

    // ★ [추가] 시퀀스 설정 (Loop 여부 등)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destruction|Sequence")
    FMovieSceneSequencePlaybackSettings SequenceSettings;

private:
    // 이미 파괴되었는지 체크
    bool bHasTriggered;

    // 사운드 페이드 아웃 시간
    float FadeOutDuration;

    // ★ [추가] 시퀀스 플레이어 포인터
    UPROPERTY()
    ULevelSequencePlayer* SequencePlayer;

    // ★ [추가] 시퀀스 종료 시 호출될 콜백 함수
    UFUNCTION()
    void OnSequenceFinished();
};