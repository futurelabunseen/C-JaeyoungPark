#include "UI/PPGASPlayerStatusUserWidget.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PPCharacterAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPPGASPlayerStatusUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	// 부모 함수 호출 (여기서 체력바가 연결됩니다!)
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		// 마나 연결 (기존 로직)
		ASC->GetGameplayAttributeValueChangeDelegate(UPPCharacterAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::OnManaChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPPCharacterAttributeSet::GetMaxManaAttribute()).AddUObject(this, &ThisClass::OnMaxManaChanged);

		// 경험치 연결
		ASC->GetGameplayAttributeValueChangeDelegate(UPPCharacterAttributeSet::GetExperienceAttribute()).AddUObject(this, &ThisClass::OnExperienceChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPPCharacterAttributeSet::GetMaxExperienceAttribute()).AddUObject(this, &ThisClass::OnMaxExperienceChanged);

		// 마나바 색상 설정
		if (PbMpBar)
		{
			PbMpBar->SetFillColorAndOpacity(ManaColor);
		}

		// 초기값 설정
		const UPPCharacterAttributeSet* CurrentAttributeSet = ASC->GetSet<UPPCharacterAttributeSet>();
		if (CurrentAttributeSet)
		{
			// 마나 초기값
			CurrentMana = CurrentAttributeSet->GetMana();
			CurrentMaxMana = CurrentAttributeSet->GetMaxMana();

			// [추가] 경험치 초기값
			CurrentExperience = CurrentAttributeSet->GetExperience();
			CurrentMaxExperience = CurrentAttributeSet->GetMaxExperience();

			if (CurrentMaxMana > 0.0f)
			{
				UpdateMpBar();
			}

			// [추가] 경험치바 초기 업데이트
			if (CurrentMaxExperience > 0.0f)
			{
				UpdateExpBar();
			}
		}
	}
}

// --- Mana Implementation ---

void UPPGASPlayerStatusUserWidget::OnManaChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMana = ChangeData.NewValue;
	UpdateMpBar();
}

void UPPGASPlayerStatusUserWidget::OnMaxManaChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxMana = ChangeData.NewValue;
	UpdateMpBar();
}

void UPPGASPlayerStatusUserWidget::UpdateMpBar()
{
	if (CurrentMaxMana <= 0.0f) return;

	if (PbMpBar)
	{
		PbMpBar->SetPercent(CurrentMana / CurrentMaxMana);
	}

	if (TxtMpStat)
	{
		TxtMpStat->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), CurrentMana, CurrentMaxMana)));
	}
}

// --- Experience Implementation ---

void UPPGASPlayerStatusUserWidget::OnExperienceChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentExperience = ChangeData.NewValue;
	UpdateExpBar();
}

void UPPGASPlayerStatusUserWidget::OnMaxExperienceChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxExperience = ChangeData.NewValue;
	UpdateExpBar();
}

void UPPGASPlayerStatusUserWidget::UpdateExpBar()
{
	if (CurrentMaxExperience <= 0.0f) return;

	if (PbExpBar)
	{
		// 경험치 퍼센트 적용
		PbExpBar->SetPercent(CurrentExperience / CurrentMaxExperience);
	}

	if (TxtExpStat)
	{
		// 텍스트 적용 (예: 50/100)
		TxtExpStat->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), CurrentExperience, CurrentMaxExperience)));
	}
}