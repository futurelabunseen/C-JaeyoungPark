// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PPCharacter.h"
#include "PPCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API APPCharacterNonPlayer : public APPCharacter
{
	GENERATED_BODY()
	
public:
	APPCharacterNonPlayer();

protected:
	virtual void SetDead() override;

	FTimerHandle DeadTimerHandle;
};
