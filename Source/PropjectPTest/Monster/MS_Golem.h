
#pragma once

#include "CoreMinimal.h"
#include "Interface/MSAIInterface.h"
#include "Character/PPGASCharacterNonPlayer.h"
#include "GameplayTagContainer.h"
#include "MS_Golem.generated.h"

UCLASS()
class PROPJECTPTEST_API AMS_Golem : public APPGASCharacterNonPlayer, public IMSAIInterface
{
	GENERATED_BODY()

public:
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

	// 경험치 보상용 GE 클래스
	UPROPERTY(EditAnywhere, Category = "GAS|Reward")
	TSubclassOf<class UGameplayEffect> ExpRewardEffectClass;

	// 이 몬스터가 주는 경험치 양
	UPROPERTY(EditAnywhere, Category = "GAS|Reward")
	float ExpRewardAmount = 10.0f;
};
