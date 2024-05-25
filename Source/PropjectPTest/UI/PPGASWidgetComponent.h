// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PPGASWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API UPPGASWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	virtual void InitWidget() override;
};
