// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PPGASWeaponBox.h"
#include "AbilitySystemBlueprintLibrary.h"

void APPGASWeaponBox::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, WeaponEventTag, FGameplayEventData());
}
