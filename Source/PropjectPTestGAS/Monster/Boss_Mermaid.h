// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/BossAIInterface.h"
#include "Character/PPGASCharacterNonPlayer.h"
#include "Boss_Mermaid.generated.h"

UCLASS()
class PROPJECTPTESTGAS_API ABoss_Mermaid : public APPGASCharacterNonPlayer, public IBossAIInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss_Mermaid();

public:

	// AI Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> AttackMontage;

	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;

	FAICharacterAttackFinished OnAttackFinished;

	// virtual void NotifyComboActionEnd() override;
};
