
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
        // ��ä�� ��� ����
        FCollisionQueryParams Params_Boss(SCENE_QUERY_STAT(UPPTA_BossAttackTrace), false, NonPlayerCharacter);

        // �ٴڿ� �پ��ִ� ���� Ʈ���̽��� �����ϱ� ���� �ణ �Ʒ������� �������� �߰�
        // ���̸� �ø��� ���� �߰� ���� ����
        float HeightOffset = NonPlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 60.0f; // 60.0f ��ŭ �� �ø�
        FVector StartOffset = Start - FVector(0, 0, HeightOffset);

        // Forward ���͸� �������� ��ä�� ������ ����
        FVector RightVector = Forward.RotateAngleAxis(AttackAngle / 2.0f, FVector::UpVector);
        FVector LeftVector = Forward.RotateAngleAxis(-AttackAngle / 2.0f, FVector::UpVector);

        // RightVector�� LeftVector�� ���� ���
        FVector RightEnd = StartOffset + RightVector * AttackRadius;
        FVector LeftEnd = StartOffset + LeftVector * AttackRadius;

        // RightEnd�� LeftEnd�� �����ϴ� ��ä�� ������ ����
        GetWorld()->SweepMultiByChannel(OutHitResults, StartOffset, StartOffset + Forward * AttackRadius, FQuat::Identity, CCHANNEL_PPACTION, FCollisionShape::MakeSphere(AttackRadius), Params_Boss);

        // ��ä�� ���� ������ ���� ��ä�ÿ� �ش��ϴ� �浹 ��ü�� ���͸�
        TArray<FHitResult> FilteredHitResults;
        for (const FHitResult& HitResult : OutHitResults)
        {
            FVector HitDirection = HitResult.ImpactPoint - StartOffset;
            HitDirection.Normalize();
            float HitAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Forward, HitDirection)));
            if (HitAngle <= AttackAngle / 2.0f)
            {
                FilteredHitResults.Add(HitResult);
            }
        }

#if ENABLE_DRAW_DEBUG
        if (bShowDebug)
        {
            DrawDebugLine(GetWorld(), StartOffset, RightEnd, FColor::Green, false, 5.0f, 0, 1.0f);
            DrawDebugLine(GetWorld(), StartOffset, LeftEnd, FColor::Green, false, 5.0f, 0, 1.0f);
            DrawDebugLine(GetWorld(), RightEnd, LeftEnd, FColor::Green, false, 5.0f, 0, 1.0f);
        }
#endif

        OutHitResults = FilteredHitResults;
    }
    else
    {
        // ĸ�� ������ ���� ����
        const FVector End_Boss = Start + Forward * ForwardLength;
        const float AttackHalfHeight_Boss = BossAttributeSet->GetAttackRange() / 2.0f;
        const FCollisionShape CollisionShape_Boss = FCollisionShape::MakeCapsule(AttackRadius, AttackHalfHeight_Boss);

        FCollisionQueryParams Params_Boss(SCENE_QUERY_STAT(UPPTA_BossAttackTrace), false, NonPlayerCharacter);
        GetWorld()->SweepMultiByChannel(OutHitResults, Start, End_Boss, FQuat::Identity, CCHANNEL_PPACTION, CollisionShape_Boss, Params_Boss);

#if ENABLE_DRAW_DEBUG
        if (bShowDebug)
        {
            for (const FHitResult& HitResult : OutHitResults)
            {
                FVector CapsuleOrigin = HitResult.ImpactPoint;
                DrawDebugCapsule(GetWorld(), CapsuleOrigin, AttackHalfHeight_Boss, AttackRadius, FRotationMatrix::MakeFromZ(Forward).ToQuat(), FColor::Green, false, 5.0f);
            }
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