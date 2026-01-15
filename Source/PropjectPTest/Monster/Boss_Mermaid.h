
#pragma once

#include "CoreMinimal.h"
#include "Interface/BossAIInterface.h"
#include "Character/PPGASCharacterNonPlayer.h"
#include "Boss_Mermaid.generated.h"

UCLASS()
class PROPJECTPTEST_API ABoss_Mermaid : public APPGASCharacterNonPlayer, public IBossAIInterface
{
	GENERATED_BODY()

public:
	ABoss_Mermaid();

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnOutOfHealth(AActor* Killer) override;

	void DisconnectFromServer();

	// 멀티캐스트 
	// 
	// 선언
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHidePlayerHUDsRPC();

	// AI Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> AttackMontage;

	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	// 경험치 보상용 GE 클래스
	UPROPERTY(EditAnywhere, Category = "GAS|Reward")
	TSubclassOf<class UGameplayEffect> ExpRewardEffectClass;

	// 보스가 주는 경험치 양
	UPROPERTY(EditAnywhere, Category = "GAS|Reward")
	float ExpRewardAmount = 1000.0f;
};
