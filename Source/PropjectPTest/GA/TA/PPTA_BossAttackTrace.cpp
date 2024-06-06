
#include "GA/TA/PPTA_BossAttackTrace.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "Character/PPGASCharacterNonPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Physics/PPCollision.h"
#include "DrawDebugHelpers.h"
#include "AbilitySystemComponent.h"
#include "Attribute/BossAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "PropjectPTest.h"

APPTA_BossAttackTrace::APPTA_BossAttackTrace()
{
}

void APPTA_BossAttackTrace::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void APPTA_BossAttackTrace::ConfirmTargetingAndContinue()
{
	if (SourceActor)
	{
		FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
		TargetDataReadyDelegate.Broadcast(DataHandle);
	}
}

FGameplayAbilityTargetDataHandle APPTA_BossAttackTrace::MakeTargetData() const
{
	APPGASCharacterNonPlayer* NonPlayerCharacter = CastChecked<APPGASCharacterNonPlayer>(SourceActor);

	if (!NonPlayerCharacter)
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

	const UBossAttributeSet* BossAttributeSet = ASC->GetSet<UBossAttributeSet>();

	if (!BossAttributeSet)
	{
		PPGAS_LOG(LogPPGAS, Error, TEXT("BossAttributeSet not found!"));
		return FGameplayAbilityTargetDataHandle();
	}

	const FVector Start = NonPlayerCharacter->GetActorLocation();
	const FVector Forward = NonPlayerCharacter->GetActorForwardVector();
	// const FVector End_Boss = Start + Forward * BossAttributeSet->GetAttackRange();
	const FVector End_Boss = Start + Forward * 1000.0f;

	TArray<FHitResult> OutHitResults;
	const float AttackRadius_Boss = BossAttributeSet->GetAttackRadius();
	const float AttackHalfHeight_Boss = BossAttributeSet->GetAttackRange() / 2.0f;

	const FCollisionShape CollisionShape_Boss = FCollisionShape::MakeCapsule(AttackRadius_Boss, AttackHalfHeight_Boss);

	FCollisionQueryParams Params_Boss(SCENE_QUERY_STAT(UPPTA_BossAttackTrace), false, NonPlayerCharacter);
	GetWorld()->SweepMultiByChannel(OutHitResults, Start, End_Boss, FQuat::Identity, CCHANNEL_PPACTION, CollisionShape_Boss, Params_Boss);

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
			DrawDebugCapsule(GetWorld(), CapsuleOrigin, AttackHalfHeight_Boss, AttackRadius_Boss, FRotationMatrix::MakeFromZ(Forward).ToQuat(), FColor::Green, false, 5.0f);
		}
	}
#endif

	return DataHandle;
}
