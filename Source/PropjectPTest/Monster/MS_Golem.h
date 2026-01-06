
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/MSAIInterface.h"
#include "Character/PPGASCharacterNonPlayer.h"
#include "GameplayTagContainer.h"
#include "MS_Golem.generated.h"

UCLASS()
class PROPJECTPTEST_API AMS_Golem : public APPGASCharacterNonPlayer, public IMSAIInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMS_Golem();

	virtual void PossessedBy(AController* NewController) override;

	// AI Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> AttackMontage;

	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	void ApplyEffectToTarget(AActor* Target);

	void InvokeGameplayCue(AActor* Target);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = GAS, Meta = (Categories = GameplayCue))
	FGameplayTag GameplayCueTag;
	virtual void OnOutOfHealth(AActor* Killer) override;

	// [추가] 경험치 보상용 GE 클래스
	UPROPERTY(EditAnywhere, Category = "GAS|Reward")
	TSubclassOf<class UGameplayEffect> ExpRewardEffectClass;

	// [추가] 이 몬스터가 주는 경험치 양
	UPROPERTY(EditAnywhere, Category = "GAS|Reward")
	float ExpRewardAmount = 10.0f;


	// 애니메이션 종료 대기용 타이머 핸들
	/*UPROPERTY()
	FTimerHandle DeadAnimTimerHandle;*/

	// 애니메이션이 끝나면 실행될 함수 (디졸브 시작)
	//void StartDissolveSequence();

	// 서버에서 모든 클라이언트에게 "즉시 죽어라" 명령을 내리는 함수
	/*UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartDeath();*/
};
