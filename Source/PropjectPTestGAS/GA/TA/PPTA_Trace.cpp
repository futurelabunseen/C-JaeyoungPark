// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/TA/PPTA_Trace.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Physics/PPCollision.h"
#include "DrawDebugHelpers.h"
#include "AbilitySystemComponent.h"
#include "Attribute/PPCharacterAttributeSet.h"
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

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	if (!ASC)
	{
		PPGAS_LOG(LogPPGAS, Error, TEXT("ASC not found!"));
		return FGameplayAbilityTargetDataHandle();
	}

	const UPPCharacterAttributeSet* AttributeSet = ASC->GetSet<UPPCharacterAttributeSet>();
	if (!AttributeSet)
	{
		PPGAS_LOG(LogPPGAS, Error, TEXT("PPCharacterAttributeSet not found!"));
		return FGameplayAbilityTargetDataHandle();
	}

	const FVector Start = Character->GetActorLocation();
	const FVector Forward = Character->GetActorForwardVector();
	const FVector End = Start + Forward * AttributeSet->GetAttackRange();

	TArray<FHitResult> OutHitResults;
	const float AttackRadius = AttributeSet->GetAttackRadius();
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(AttackRadius);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(UPPTA_Trace), false, Character);

	GetWorld()->SweepMultiByChannel(OutHitResults, Start, End, FQuat::Identity, CCHANNEL_PPACTION, CollisionShape, Params);

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
			float CapsuleHalfHeight = AttackRadius;
			DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(Forward).ToQuat(), FColor::Green, false, 5.0f);
		}
	}
#endif

	return DataHandle;
}

