// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PPCharacterNonPlayer.h"
#include "AbilitySystemInterface.h"
#include "PPGASCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTESTGAS_API APPGASCharacterNonPlayer : public APPCharacterNonPlayer, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	APPGASCharacterNonPlayer();
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION()
	virtual void OnOutOfHealth();

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<class UPPCharacterAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<class UGameplayEffect> InitStatEffect;

	UPROPERTY(EditAnywhere, Category = GAS)
	float Level;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UPPGASWidgetComponent> HpBar;
};
