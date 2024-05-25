// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/PPGASItemBox.h"
#include "PPGASWeaponBox.generated.h"

/**
 * 
 */
UCLASS()
class PROPJECTPTEST_API APPGASWeaponBox : public APPGASItemBox
{
	GENERATED_BODY()
	
protected:
	virtual void NotifyActorBeginOverlap(AActor* Other) override;

protected:
	UPROPERTY(EditAnywhere, Category = GAS, Meta=(Categories=Event))
	FGameplayTag WeaponEventTag;
};
