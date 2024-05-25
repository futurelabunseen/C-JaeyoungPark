// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "PPGC_InteractEffect.generated.h"

/**
 *
 */
UCLASS()
class PROPJECTPTEST_API UPPGC_InteractEffect : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	UPPGC_InteractEffect();

	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<class UParticleSystem> ParticleSystem;
};
