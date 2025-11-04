#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InterestManager.generated.h"

// AI의 세 가지 상태를 정의합니다.
UENUM(BlueprintType)
enum class EAIState : uint8
{
    Dormant,    // 휴면 상태 (액터가 스폰되지 않음)
    Relevant,   // 관찰 상태 (단순 로직, 저품질 애셋)
    Active      // 활성 상태 (모든 기능 사용)
};

// 휴면 상태의 AI 정보를 담을 구조체
USTRUCT(BlueprintType)
struct FMonsterProxyData
{
    GENERATED_BODY()

    // 몬스터의 종류 (어떤 클래스를 스폰할지)
    UPROPERTY(EditAnywhere)
    TSubclassOf<class APPGASCharacterNonPlayer> MonsterClass; // 몬스터의 공통 부모 클래스로 지정

    // 마지막으로 알려진 위치 (휴면 상태가 될 때의 위치)
    UPROPERTY(VisibleAnywhere)
    FVector LastKnownLocation;

    // 현재 AI의 상태
    UPROPERTY(VisibleAnywhere)
    EAIState CurrentState = EAIState::Dormant;

    // 스폰된 액터에 대한 포인터 (휴면 상태일 때는 nullptr)
    // TWeakObjectPtr는 액터가 파괴되었을 때 자동으로 nullptr로 바뀌어 안전합니다.
    TWeakObjectPtr<class APPGASCharacterNonPlayer> SpawnedActorPtr;
};

UCLASS()
class PROPJECTPTEST_API AInterestManager : public AActor
{
    GENERATED_BODY()

public:
    AInterestManager();

    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // ◀ 추가

protected:
    virtual void BeginPlay() override;
    void UpdateMonstersState(); // 주기적으로 호출될 메인 함수
    void InitializeManager();

    void DrawDebugInterestZones(const FVector& Location);

    // 몬스터를 풀에서 가져와 활성화
    class APPGASCharacterNonPlayer* SpawnMonsterFromPool(const FMonsterProxyData& ProxyData, EAIState DesiredState);
    // 몬스터를 비활성화하여 풀에 반납
    void DespawnMonsterToPool(class APPGASCharacterNonPlayer* MonsterToDespawn);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InterestManager|Spawn")
    float SpawnSearchRadius = 500.0f; // [수정] 스폰 위치를 찾기 위한 탐색 반경입니다.

private:
    // 월드의 모든 몬스터 정보를 담을 배열
    UPROPERTY(EditAnywhere, Category = "Interest Settings")
    TArray<FMonsterProxyData> MonsterProxies;

    // 재활용할 몬스터 액터를 담아두는 오브젝트 풀
    UPROPERTY()
    TArray<TObjectPtr<class APPGASCharacterNonPlayer>> MonsterPool;

    // 상태 변경 거리 설정
    UPROPERTY(EditAnywhere, Category = "Interest Settings")
    float ActiveRadius = 3000.f; // 이 거리 안으로 들어오면 Active 상태

    UPROPERTY(EditAnywhere, Category = "Interest Settings")
    float RelevantRadius = 8000.f; // 이 거리 안으로 들어오면 Relevant 상태

    UPROPERTY(EditAnywhere, Category = "Interest Settings", meta = (AllowPrivateAccess = "true"))
    float VerticalTolerance = 500.0f;

    UPROPERTY(EditAnywhere, Category = "Interest Settings")
    float UpdateInterval = 1.0f; // 상태 업데이트 주기 (초)

    FTimerHandle UpdateTimerHandle;
    FTimerHandle InitTimerHandle; // 초기화를 위한 타이머 핸들 추가

    UPROPERTY(EditAnywhere, Category = "Debug")
    bool bShowDebugSpheres = true;
};