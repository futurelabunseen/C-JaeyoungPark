// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PPGASUserWidget.h"
#include "AbilitySystemBlueprintLibrary.h"

void UPPGASUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	if (IsValid(InOwner))
	{
		UE_LOG(LogTemp, Warning, TEXT("aaaaaaaa"));
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner);
	}
	UE_LOG(LogTemp, Warning, TEXT("bbbbbbb"));
}

UAbilitySystemComponent* UPPGASUserWidget::GetAbilitySystemComponent() const
{
	UE_LOG(LogTemp, Warning, TEXT("%d"), ASC);
	return ASC;
}
