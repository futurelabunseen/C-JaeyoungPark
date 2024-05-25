// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/TA/PPTA_Trace.h"
#include "PPTA_SphereMultiTrace.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API APPTA_SphereMultiTrace : public APPTA_Trace
{
	GENERATED_BODY()

protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const override;
};
