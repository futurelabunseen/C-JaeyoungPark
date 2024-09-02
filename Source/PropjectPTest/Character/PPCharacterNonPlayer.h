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

	// ��Ƽ���� �Ķ���� �÷��� �ν��Ͻ�
	// UMaterialParameterCollectionInstance* ParameterCollectionInstance;

	// ������ ȿ���� ���� ��Ƽ���� �Ķ���� �÷���
	UPROPERTY(EditAnywhere, Category = "Effects")
	UMaterialParameterCollection* MyMaterialParameterCollection;

	void TriggerDissolveEffect();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//// ������ ��Ƽ���� ������ ���� ���� (�������Ʈ���� ���� ����)
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials", meta = (AllowPrivateAccess = "true"))
	//UMaterialInterface* DissolveMaterial;
};
