
#pragma once

#include "GameplayTagContainer.h"

#define PPTAG_DATA_DAMAGE FGameplayTag::RequestGameplayTag(FName("Data.Damage"))
#define PPTAG_CHARACTER_ISDEAD FGameplayTag::RequestGameplayTag(FName("Character.State.IsDead"))
#define PPTAG_CHARACTER_INVINSIBLE FGameplayTag::RequestGameplayTag(FName("Character.State.Invinsible"))
#define PPTAG_CHARACTER_INTERACTION FGameplayTag::RequestGameplayTag(FName("Character.State.CanInteraction"))
#define PPTAG_CHARACTER_INTERACTIONING FGameplayTag::RequestGameplayTag(FName("Character.State.ISInteractioning"))
#define PPTAG_GAMEPLAYCUE_CHARACTER_ATTACKHIT FGameplayTag::RequestGameplayTag(FName("GameplayCue.Character.AttackHit"))

