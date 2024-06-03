// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/PPGASUserWidget.h"
#include "GameplayEffectTypes.h"
#include "PPGASHpBarUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API UPPGASHpBarUserWidget : public UPPGASUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHpBar();
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

protected:
	virtual void OnHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	/*virtual void OnSkillEnergyChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxSkillEnergyChanged(const FOnAttributeChangeData& ChangeData);*/
	virtual void OnInvinsibleTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> PbHpBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtHpStat;

	/*UPROPERTY()
	TObjectPtr<class UProgressBar> PbMpBar;

	UPROPERTY()
	TObjectPtr<class UTextBlock> TxtMpStat;*/

	float CurrentHealth = 0.0f;
	float CurrentMaxHealth = 0.1f;

	/*float CurrentSkillEnergy = 0.0f;
	float CurrentMaxSkillEnergy = 0.1f;*/
	
	FLinearColor HealthColor = FLinearColor::Red;
	// FLinearColor SkillEnergyColor = FLinearColor::Blue;
	FLinearColor InvinsibleColor = FLinearColor::Blue;
};
