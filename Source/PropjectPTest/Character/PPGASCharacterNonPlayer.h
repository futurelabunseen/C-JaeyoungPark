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
class PROPJECTPTEST_API APPGASCharacterNonPlayer : public APPCharacterNonPlayer, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	APPGASCharacterNonPlayer();
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION()
	virtual void OnOutOfHealth();

	UPROPERTY()
	TObjectPtr<class UMonsterAttributeSet> MonsterAttributeSet;

	UPROPERTY()
	TObjectPtr<class UBossAttributeSet> BossAttributeSet;

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<class UGameplayEffect> InitStatEffect;

	UPROPERTY(EditAnywhere, Category = GAS)
	float Level = 1;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UPPGASWidgetComponent> HpBar;

	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, Category = GAS)
	TMap<int32, TSubclassOf<class UGameplayAbility>> StartInputAbilities;

};
