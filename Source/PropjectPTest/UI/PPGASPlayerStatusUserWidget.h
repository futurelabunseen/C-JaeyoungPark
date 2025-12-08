// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/PPGASHpBarUserWidget.h"
#include "PPGASPlayerStatusUserWidget.generated.h"

/**
 * 부모 클래스(HpBarUserWidget)가 체력을 담당하고,
 * 이 클래스는 마나와 경험치를 추가로 담당합니다.
 */
UCLASS()
class PROPJECTPTEST_API UPPGASPlayerStatusUserWidget : public UPPGASHpBarUserWidget
{
	GENERATED_BODY()
public:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

	void UpdateMpBar();
	void UpdateExpBar(); // [추가] 경험치바 업데이트 함수

protected:
	// --- Mana Section ---
	void OnManaChanged(const FOnAttributeChangeData& ChangeData);
	void OnMaxManaChanged(const FOnAttributeChangeData& ChangeData);

	// --- Experience Section ---
	void OnExperienceChanged(const FOnAttributeChangeData& ChangeData);
	void OnMaxExperienceChanged(const FOnAttributeChangeData& ChangeData);

protected:
	// --- Widgets ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> PbMpBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtMpStat;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> PbExpBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtExpStat;

	// --- Values ---
	float CurrentMana = 0.0f;
	float CurrentMaxMana = 0.0f;

	float CurrentExperience = 0.0f;
	float CurrentMaxExperience = 0.0f;

	FLinearColor ManaColor = FLinearColor::Blue;
};