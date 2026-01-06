
#pragma once

#include "CoreMinimal.h"
#include "UI/PPGASHpBarUserWidget.h"
#include "PPGASPlayerStatusUserWidget.generated.h"

UCLASS()
class PROPJECTPTEST_API UPPGASPlayerStatusUserWidget : public UPPGASHpBarUserWidget
{
	GENERATED_BODY()
public:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

	void UpdateMpBar();
	void UpdateExpBar(); // 경험치바 업데이트 함수

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