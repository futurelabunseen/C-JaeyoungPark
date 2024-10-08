// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "PPGASPlayerState.generated.h"

/**
 *
 */
UCLASS()
class PROPJECTPTEST_API APPGASPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APPGASPlayerState();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:


	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<class UPPCharacterAttributeSet> AttributeSet;

	UPROPERTY()
	TObjectPtr<class UPPCharacterSkillAttributeSet> SkillAttributeSet;

};
