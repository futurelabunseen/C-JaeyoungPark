// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/MSAIInterface.h"
#include "Character/PPGASCharacterNonPlayer.h"
#include "GameplayTagContainer.h"
#include "MS_Golem.generated.h"

UCLASS()
class PROPJECTPTESTGAS_API AMS_Golem : public APPGASCharacterNonPlayer, public IMSAIInterface
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

	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = GAS, Meta = (Categories = GameplayCue))
	FGameplayTag GameplayCueTag;
};
