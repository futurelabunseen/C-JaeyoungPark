// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/PPGASHpBarUserWidget.h"
#include "PPGASPlayerStatusUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API UPPGASPlayerStatusUserWidget : public UPPGASHpBarUserWidget
{
	GENERATED_BODY()
public:
	void UpdateMpBar();
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

protected:
	virtual void OnSkillEnergyChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxSkillEnergyChanged(const FOnAttributeChangeData& ChangeData);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> PbMpBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtMpStat;

	float CurrentSkillEnergy = 0.0f;
	float CurrentMaxSkillEnergy = 0.1f;

	FLinearColor SkillEnergyColor = FLinearColor::Blue;
};
