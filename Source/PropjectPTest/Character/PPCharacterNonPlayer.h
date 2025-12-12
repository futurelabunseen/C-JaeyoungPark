// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PPCharacter.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "PPCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API APPCharacterNonPlayer : public APPCharacter
{
	GENERATED_BODY()
	
public:
	APPCharacterNonPlayer();

protected:
	virtual void SetDead() override;

	FTimerHandle DeadTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* DissolveMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* DissolveEffect;

	FTimerHandle DissolveTimerHandle;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	// [추가] 디졸브 진행 속도
	float DissolveSpeed = 2.0f; // 값이 클수록 빨리 사라짐

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartDissolve();

	void StartDissolveTimeline();
	void UpdateDissolveMaterial();

	FTimerHandle DissolveTimelineTimerHandle;
	float CurrentDissolveValue = 0.0f;
};
