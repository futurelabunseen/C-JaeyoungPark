// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

	virtual void OnOutOfHealth() override;

	void DisconnectFromServer();

	// 멀티캐스트 RPC 선언
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
};
