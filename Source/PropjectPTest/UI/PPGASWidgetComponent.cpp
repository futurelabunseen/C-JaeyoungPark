// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PPGASWidgetComponent.h"
#include "UI/PPGASUserWidget.h"

void UPPGASWidgetComponent::InitWidget()
{
	Super::InitWidget();

	UPPGASUserWidget* GASUserWidget = Cast<UPPGASUserWidget>(GetWidget());
	if (GASUserWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Init Widget"));
		GASUserWidget->SetAbilitySystemComponent(GetOwner());
	}
}