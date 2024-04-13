// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PPCharacter.h"
#include "Engine/StreamableManager.h"
#include "PPCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS(config=PropjectPTest)
class PROPJECTPTEST_API APPCharacterNonPlayer : public APPCharacter
	//, public IMSAIInterface
{
	GENERATED_BODY()
	
public:
	APPCharacterNonPlayer();

protected:
	virtual void PostInitializeComponents() override;
	virtual void SetDead() override;

protected:
	void NPCMeshLoadCompleted();
	// void MonsterAttack();

	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCMeshes;
	
	TSharedPtr<FStreamableHandle> NPCMeshHandle;

	// AI Section
//protected:
//	virtual float GetAIPatrolRadius() override;
//	virtual float GetAIDetectRange() override;
//	virtual float GetAIAttackRange() override;
//	virtual float GetAITurnSpeed() override;
//
//	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
//	virtual void AttackByAI() override;
//
//	FAICharacterAttackFinished OnAttackFinished;
//
//	virtual void NotifyComboActionEnd() override;
};
