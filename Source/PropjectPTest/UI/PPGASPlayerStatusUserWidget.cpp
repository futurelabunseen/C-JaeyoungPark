// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PPGASPlayerStatusUserWidget.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PPCharacterSkillAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPPGASPlayerStatusUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UPPCharacterSkillAttributeSet::GetSkillEnergyAttribute()).AddUObject(this, &UPPGASPlayerStatusUserWidget::OnSkillEnergyChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPPCharacterSkillAttributeSet::GetMaxSkillEnergyAttribute()).AddUObject(this, &UPPGASPlayerStatusUserWidget::OnMaxSkillEnergyChanged);

		if (PbMpBar)
		{
			PbMpBar->SetFillColorAndOpacity(SkillEnergyColor);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PbMpBar is not set"));
		}

		const UPPCharacterSkillAttributeSet* CurrentPPCharacterSkillAttributeSet = ASC->GetSet<UPPCharacterSkillAttributeSet>();
		if (CurrentPPCharacterSkillAttributeSet)
		{
			CurrentSkillEnergy = CurrentPPCharacterSkillAttributeSet->GetSkillEnergy();
			CurrentMaxSkillEnergy = CurrentPPCharacterSkillAttributeSet->GetMaxSkillEnergy();

			if (CurrentMaxSkillEnergy > 0.0f)
			{
				UpdateMpBar();
			}
		}
	}
}

void UPPGASPlayerStatusUserWidget::OnSkillEnergyChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentSkillEnergy = ChangeData.NewValue;
	// const UPPCharacterSkillAttributeSet* CurrentPPCharacterSkillAttributeSet = ASC->GetSet<UPPCharacterSkillAttributeSet>();
	/*if (CurrentPPCharacterSkillAttributeSet)
	{
		UpdateMpBar();
	}*/

	UpdateMpBar();
}

void UPPGASPlayerStatusUserWidget::OnMaxSkillEnergyChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxSkillEnergy = ChangeData.NewValue;
	UpdateMpBar();
}

void UPPGASPlayerStatusUserWidget::UpdateMpBar()
{
	if (PbMpBar)
	{
		PbMpBar->SetPercent(CurrentSkillEnergy / CurrentMaxSkillEnergy);
	}

	if (TxtMpStat)
	{
		TxtMpStat->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%0.f"), CurrentSkillEnergy, CurrentMaxSkillEnergy)));
		 UE_LOG(LogTemp, Warning, TEXT("CurrentSkillEnergy : %f"), CurrentSkillEnergy);
		 UE_LOG(LogTemp, Warning, TEXT("CurrentMaxSkillEnergy : %f"), CurrentMaxSkillEnergy);
	}
}
