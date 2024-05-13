// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/TA/PPTA_Trace.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "Character/PPGASCharacterNonPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Physics/PPCollision.h"
#include "DrawDebugHelpers.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PPCharacterAttributeSet.h"
// #include "Attribute/MonsterAttributeSet.h"
// #include "Attribute/BossAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "PropjectPTestGAS.h"

APPTA_Trace::APPTA_Trace()
{
}

void APPTA_Trace::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void APPTA_Trace::ConfirmTargetingAndContinue()
{
	if (SourceActor)
	{
		FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
		TargetDataReadyDelegate.Broadcast(DataHandle);
	}
}

FGameplayAbilityTargetDataHandle APPTA_Trace::MakeTargetData() const
{
	ACharacter* Character = CastChecked<ACharacter>(SourceActor);
	// APPGASCharacterNonPlayer* NonPlayerCharacter = CastChecked<APPGASCharacterNonPlayer>(SourceActor);

	if (!Character)
	{
		PPGAS_LOG(LogPPGAS, Error, TEXT("SourceActor is not a Character!"));
		return FGameplayAbilityTargetDataHandle();
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	if (!ASC)
	{
		PPGAS_LOG(LogPPGAS, Error, TEXT("ASC not found!"));
		return FGameplayAbilityTargetDataHandle();
	}

	const UPPCharacterAttributeSet* CharacterAttributeSet = ASC->GetSet<UPPCharacterAttributeSet>();
	// const UMonsterAttributeSet* MonsterAttributeSet = ASC->GetSet<UMonsterAttributeSet>();
	// const UBossAttributeSet* BossAttributeSet = ASC->GetSet<UBossAttributeSet>();

	if (!CharacterAttributeSet)
	{
		PPGAS_LOG(LogPPGAS, Error, TEXT("CharacterAttributeSet not found!"));
		return FGameplayAbilityTargetDataHandle();
	}

	/*if (!MonsterAttributeSet)
	{
		PPGAS_LOG(LogPPGAS, Error, TEXT("MonsterAttributeSet not found!"));
		return FGameplayAbilityTargetDataHandle();
	}*/

	/*if (!BossAttributeSet)
	{
		PPGAS_LOG(LogPPGAS, Error, TEXT("BossAttributeSet not found!"));
		return FGameplayAbilityTargetDataHandle();
	}*/

	const FVector Start = Character->GetActorLocation();
	const FVector Forward = Character->GetActorForwardVector();

	const FVector End_Character = Start + Forward * CharacterAttributeSet->GetAttackRange();
	// const FVector End_Monster = Start + Forward * MonsterAttributeSet->GetAttackRange();
	// const FVector End_Boss = Start + Forward * BossAttributeSet->GetAttackRange();

	TArray<FHitResult> OutHitResults;
	const float AttackRadius_Character = CharacterAttributeSet->GetAttackRadius();
	// const float AttackRadius_Monster = MonsterAttributeSet->GetAttackRadius();
	// const float AttackRadius_Boss = BossAttributeSet->GetAttackRadius();

	const FCollisionShape CollisionShape_Character = FCollisionShape::MakeSphere(AttackRadius_Character);
	// const FCollisionShape CollisionShape_Monster = FCollisionShape::MakeSphere(AttackRadius_Monster);
	// const FCollisionShape CollisionShape_Boss = FCollisionShape::MakeSphere(AttackRadius_Boss);

	FCollisionQueryParams Params_Character(SCENE_QUERY_STAT(UPPTA_Trace), false, Character);
	// FCollisionQueryParams Params_Monster(SCENE_QUERY_STAT(UPPTA_Trace), false, NonPlayerCharacter);

	GetWorld()->SweepMultiByChannel(OutHitResults, Start, End_Character, FQuat::Identity, CCHANNEL_PPACTION, CollisionShape_Character, Params_Character);
	// GetWorld()->SweepMultiByChannel(OutHitResults, Start, End_Monster, FQuat::Identity, CCHANNEL_PPACTION2, CollisionShape_Monster, Params_Monster);
	// GetWorld()->SweepMultiByChannel(OutHitResults, Start, End_Boss, FQuat::Identity, CCHANNEL_PPACTION1, CollisionShape_Boss, Params);

	FGameplayAbilityTargetDataHandle DataHandle;
	for (const FHitResult& HitResult : OutHitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		DataHandle.Add(TargetData);
	}

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		for (const FHitResult& HitResult : OutHitResults)
		{
			FVector CapsuleOrigin = HitResult.ImpactPoint;
			float CapsuleHalfHeight_Character = AttackRadius_Character;
			// float CapsuleHalfHeight_Monster = AttackRadius_Monster;
			// float CapsuleHalfHeight_Boss = AttackRadius_Boss;

			
			DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight_Character, AttackRadius_Character, FRotationMatrix::MakeFromZ(Forward).ToQuat(), FColor::Green, false, 5.0f);
			// DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight_Monster, AttackRadius_Monster, FRotationMatrix::MakeFromZ(Forward).ToQuat(), FColor::Green, false, 5.0f);
			// DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight_Boss, AttackRadius_Boss, FRotationMatrix::MakeFromZ(Forward).ToQuat(), FColor::Green, false, 5.0f);
			// DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight_Character, AttackRadius_Character, FRotationMatrix::MakeFromZ(Forward).ToQuat(), FColor::Green, false, 5.0f);

		}
	}
#endif

	return DataHandle;
}

