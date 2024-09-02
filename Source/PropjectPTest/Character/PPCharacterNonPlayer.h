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

	// 머티리얼 파라미터 컬렉션 인스턴스
	// UMaterialParameterCollectionInstance* ParameterCollectionInstance;

	// 디졸브 효과를 위한 머티리얼 파라미터 컬렉션
	UPROPERTY(EditAnywhere, Category = "Effects")
	UMaterialParameterCollection* MyMaterialParameterCollection;

	void TriggerDissolveEffect();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//// 디졸브 머티리얼 참조를 위한 변수 (블루프린트에서 설정 가능)
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials", meta = (AllowPrivateAccess = "true"))
	//UMaterialInterface* DissolveMaterial;
};
