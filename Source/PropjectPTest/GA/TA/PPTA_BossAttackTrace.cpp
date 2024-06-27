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
    TArray<FHitResult> OutHitResults;

    if (AttackAngle > 0.0f)
    {
        // 부채꼴 모양 공격
        FCollisionQueryParams Params_Boss(SCENE_QUERY_STAT(UPPTA_BossAttackTrace), false, NonPlayerCharacter);

        // 바닥에 붙어있는 라인 트레이스를 생성
        float HeightOffset = NonPlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 60.0f; // 높이 60.0f 만큼 더 올림
        FVector StartOffset = Start - FVector(0, 0, HeightOffset);

        // 부채꼴 영역을 기준으로 스위핑
        GetWorld()->SweepMultiByChannel(OutHitResults, StartOffset, StartOffset + Forward * AttackRadius,
            FQuat::Identity, CCHANNEL_PPACTION, FCollisionShape::MakeSphere(AttackRadius), Params_Boss);

        // 부채꼴 영역 내에서 실제 부채꼴에 해당하는 충돌 객체를 필터링
        TArray<FHitResult> FilteredHitResults;
        for (const FHitResult& HitResult : OutHitResults)
        {
            FVector HitDirection = (HitResult.ImpactPoint - StartOffset).GetSafeNormal();
            float HitAngle = FMath::Acos(FVector::DotProduct(Forward, HitDirection));
            float DegreesHitAngle = FMath::RadiansToDegrees(HitAngle);
            if (DegreesHitAngle <= (AttackAngle / 2.0f))
            {
                FilteredHitResults.Add(HitResult);
            }
        }
#if ENABLE_DRAW_DEBUG
        if (bShowDebug)
        {
            // 부채꼴 디버그 드로잉
            DrawDebugFan(GetWorld(), StartOffset, Forward, AttackRadius, AttackAngle, FColor::Green, 5.0f);
        }
#endif
        OutHitResults = FilteredHitResults;
    }
    else
    {
        // 캡슐 형태의 공격 범위
        const FVector End_Boss = Start + Forward * ForwardLength;
        const float AttackHalfHeight_Boss = BossAttributeSet->GetAttackRange() / 2.0f;
        const FCollisionShape CollisionShape_Boss = FCollisionShape::MakeCapsule(AttackRadius, AttackHalfHeight_Boss);

        FCollisionQueryParams Params_Boss(SCENE_QUERY_STAT(UPPTA_BossAttackTrace), false, NonPlayerCharacter);
        GetWorld()->SweepMultiByChannel(OutHitResults, Start, End_Boss, FQuat::Identity, CCHANNEL_PPACTION, CollisionShape_Boss, Params_Boss);
#if ENABLE_DRAW_DEBUG
        if (bShowDebug)
        {
            DrawDebugCapsule(GetWorld(), (Start + End_Boss) / 2, AttackHalfHeight_Boss, AttackRadius, FQuat::Identity, FColor::Green, false, 5.0f);
        }
#endif
    }

    FGameplayAbilityTargetDataHandle DataHandle;
    for (const FHitResult& HitResult : OutHitResults)
    {
        FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
        DataHandle.Add(TargetData);
    }
    return DataHandle;
}

void APPTA_BossAttackTrace::DrawDebugFan(UWorld* World, const FVector& Start, const FVector& Forward, float Radius, float Angle, const FColor& Color, float Duration, bool bPersistentLines, uint8 DepthPriority, float Thickness) const
{
    // Number of segments to draw the fan
    const int32 NumSegments = 20;
    const float AngleStep = Angle / NumSegments;

    FVector LastPoint = Start + Forward.RotateAngleAxis(-Angle / 2.0f, FVector::UpVector) * Radius;
    for (int32 i = 1; i <= NumSegments; ++i)
    {
        float CurrentAngle = -Angle / 2.0f + i * AngleStep;
        FVector CurrentVector = Forward.RotateAngleAxis(CurrentAngle, FVector::UpVector);
        FVector CurrentPoint = Start + CurrentVector * Radius;

        // Draw the line segment
        DrawDebugLine(World, LastPoint, CurrentPoint, Color, bPersistentLines, Duration, DepthPriority, Thickness);
        LastPoint = CurrentPoint;
    }

    // Draw the base lines
    DrawDebugLine(World, Start, Start + Forward.RotateAngleAxis(Angle / 2.0f, FVector::UpVector) * Radius, Color, bPersistentLines, Duration, DepthPriority, Thickness);
    DrawDebugLine(World, Start, Start + Forward.RotateAngleAxis(-Angle / 2.0f, FVector::UpVector) * Radius, Color, bPersistentLines, Duration, DepthPriority, Thickness);
}