// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PPGASHpBarUserWidget.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PPCharacterAttributeSet.h"
// #include "Attribute/PPCharacterSkillAttributeSet.h"
#include "Attribute/MonsterAttributeSet.h"
#include "Attribute/BossAttributeSet.h"
#include "PropjectPTest.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Tag/PPGameplayTag.h"

void UPPGASHpBarUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *ASC->GetOwner()->GetName());
		ASC->GetGameplayAttributeValueChangeDelegate(UPPCharacterAttributeSet::GetHealthAttribute()).AddUObject(this, &UPPGASHpBarUserWidget::OnHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPPCharacterAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UPPGASHpBarUserWidget::OnMaxHealthChanged);

		/*ASC->GetGameplayAttributeValueChangeDelegate(UPPCharacterSkillAttributeSet::GetSkillEnergyAttribute()).AddUObject(this, &UPPGASHpBarUserWidget::OnSkillEnergyChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UPPCharacterSkillAttributeSet::GetMaxSkillEnergyAttribute()).AddUObject(this, &UPPGASHpBarUserWidget::OnMaxSkillEnergyChanged);*/

		ASC->RegisterGameplayTagEvent(PPTAG_CHARACTER_INVINSIBLE, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPPGASHpBarUserWidget::OnInvinsibleTagChanged);

		ASC->GetGameplayAttributeValueChangeDelegate(UMonsterAttributeSet::GetHealthAttribute()).AddUObject(this, &UPPGASHpBarUserWidget::OnHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UMonsterAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UPPGASHpBarUserWidget::OnMaxHealthChanged);

		ASC->GetGameplayAttributeValueChangeDelegate(UBossAttributeSet::GetHealthAttribute()).AddUObject(this, &UPPGASHpBarUserWidget::OnHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UBossAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UPPGASHpBarUserWidget::OnMaxHealthChanged);
		
		if (PbHpBar)
		{
			PbHpBar->SetFillColorAndOpacity(HealthColor);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PbHpBar is not set"));
		}

		/*if (PbMpBar)
		{
			PbMpBar->SetFillColorAndOpacity(SkillEnergyColor);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PbMpBar is not set"));
		}*/

		const UPPCharacterAttributeSet* CurrentPPCharacterAttributeSet = ASC->GetSet<UPPCharacterAttributeSet>();
		// const UPPCharacterSkillAttributeSet* CurrentPPCharacterSkillAttributeSet = ASC->GetSet<UPPCharacterSkillAttributeSet>();
		if (CurrentPPCharacterAttributeSet)
		{
			CurrentHealth = CurrentPPCharacterAttributeSet->GetHealth();
			CurrentMaxHealth = CurrentPPCharacterAttributeSet->GetMaxHealth();

			/*CurrentSkillEnergy = CurrentPPCharacterSkillAttributeSet->GetSkillEnergy();
			CurrentMaxSkillEnergy = CurrentPPCharacterSkillAttributeSet->GetMaxSkillEnergy();*/

			if (CurrentMaxHealth > 0.0f)
			{
				UpdateHpBar();
			}
		}

		const UMonsterAttributeSet* CurrentMonsterAttributeSet = ASC->GetSet<UMonsterAttributeSet>();
		if (CurrentMonsterAttributeSet)
		{
			CurrentHealth = CurrentMonsterAttributeSet->GetHealth();
			CurrentMaxHealth = CurrentMonsterAttributeSet->GetMaxHealth();

			if (CurrentMaxHealth > 0.0f)
			{
				UpdateHpBar();
			}
		}

		const UBossAttributeSet* CurrentBossAttributeSet = ASC->GetSet<UBossAttributeSet>();
		if (CurrentBossAttributeSet)
		{
			CurrentHealth = CurrentBossAttributeSet->GetHealth();
			CurrentMaxHealth = CurrentBossAttributeSet->GetMaxHealth();

			if (CurrentMaxHealth > 0.0f)
			{
				UpdateHpBar();
			}
		}
	}
}

void UPPGASHpBarUserWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentHealth = ChangeData.NewValue;
	const UPPCharacterAttributeSet* CurrentPPCharacterAttributeSet = ASC->GetSet<UPPCharacterAttributeSet>();
	UpdateHpBar();
}

void UPPGASHpBarUserWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxHealth = ChangeData.NewValue;
	UpdateHpBar();
}


//void UPPGASHpBarUserWidget::OnSkillEnergyChanged(const FOnAttributeChangeData& ChangeData)
//{
//	CurrentSkillEnergy = ChangeData.NewValue;
//	const UPPCharacterSkillAttributeSet* CurrentPPCharacterSkillAttributeSet = ASC->GetSet<UPPCharacterSkillAttributeSet>();
//	UpdateHpBar();
//}
//
//void UPPGASHpBarUserWidget::OnMaxSkillEnergyChanged(const FOnAttributeChangeData& ChangeData)
//{
//	CurrentMaxSkillEnergy = ChangeData.NewValue;
//	UpdateHpBar();
//}

void UPPGASHpBarUserWidget::OnInvinsibleTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		PbHpBar->SetFillColorAndOpacity(InvinsibleColor);
		PbHpBar->SetPercent(1.0f);
	}
	else
	{
		PbHpBar->SetFillColorAndOpacity(HealthColor);
		UpdateHpBar();
	}
}

void UPPGASHpBarUserWidget::UpdateHpBar()
{
	if (PbHpBar)
	{
		PbHpBar->SetPercent(CurrentHealth / CurrentMaxHealth);
	}

	/*if (PbMpBar)
	{
		PbMpBar->SetPercent(CurrentSkillEnergy / CurrentMaxSkillEnergy);
	}*/

	if (TxtHpStat)
	{
		TxtHpStat->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%0.f"), CurrentHealth, CurrentMaxHealth)));
		// UE_LOG(LogTemp, Warning, TEXT("CurrentHealth : %f"), CurrentHealth);
		// UE_LOG(LogTemp, Warning, TEXT("CurrentMaxHealth : %f"), CurrentMaxHealth);
	}

	//if (TxtMpStat)
	//{
	//	TxtMpStat->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%0.f"), CurrentSkillEnergy, CurrentMaxSkillEnergy)));
	//	// UE_LOG(LogTemp, Warning, TEXT("CurrentSkillEnergy : %f"), CurrentSkillEnergy);
	//	// UE_LOG(LogTemp, Warning, TEXT("CurrentMaxSkillEnergy : %f"), CurrentMaxSkillEnergy);
	//}
}
