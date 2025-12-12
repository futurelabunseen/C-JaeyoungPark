#include "Character/PPCharacterNonPlayer.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h" // For networking functions
#include "NiagaraComponent.h"

APPCharacterNonPlayer::APPCharacterNonPlayer()
{
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // 나이아가라 이펙트를 로드할 수 있도록 준비
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> DissolveEffectFinder(TEXT("/Game/Material/NS_EdgeDissolve.NS_EdgeDissolve"));
    if (DissolveEffectFinder.Succeeded())
    {
        DissolveEffect = DissolveEffectFinder.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Niagara Effect is Not Set"));
    }
}

void APPCharacterNonPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 여기에 필요한 변수를 추가로 등록할 수 있습니다.
}

void APPCharacterNonPlayer::SetDead()
{
    // 1. 상위 클래스(APPCharacter)의 SetDead 호출 -> 애니메이션 재생 및 상태 플래그 변경
    Super::SetDead();

    // 2. AI 및 움직임 정지
    AAIController* AIController = Cast<AAIController>(GetController());
    if (IsValid(AIController))
    {
        AIController->StopMovement();
        AIController->UnPossess();
    }

    if (HasAuthority())
    {
        // 3. 타이머 시간 계산
        float AnimLength = 1.5f; // 애니메이션이 없을 경우를 대비한 기본값
        if (GetDeadMontage())
        {
            // 몽타주 길이 가져오기 (0.1초 정도 여유를 두어 끊김 방지)
            AnimLength = GetDeadMontage()->GetPlayLength() - 0.1f;
        }

        // [타이머 1] 애니메이션이 끝날 때쯤 -> 디졸브 시작 (Multicast 호출)
        GetWorld()->GetTimerManager().SetTimer(DissolveTimerHandle, this, &APPCharacterNonPlayer::Multicast_StartDissolve, AnimLength, false);

        // [타이머 2] 파괴 예약
        // 중요: "애니메이션 시간(AnimLength)" + "디졸브 보여줄 시간(DeadEventDelayTime)" 후에 파괴합니다.
        // 이렇게 하면 애니메이션 도중 사라지는 문제를 100% 방지할 수 있습니다.
        float TotalDestroyDelay = AnimLength + DeadEventDelayTime;

        GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
            [&]()
            {
                Destroy();
            }
        ), TotalDestroyDelay, false);
    }
}

void APPCharacterNonPlayer::Multicast_StartDissolve_Implementation()
{
    // 1. 나이아가라 이펙트(파티클) 실행
    if (DissolveEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            DissolveEffect,
            GetMesh(),
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset,
            true);
    }

    // 2. 몬스터 몸체(Mesh) 녹이기 시작
    StartDissolveTimeline();
}

void APPCharacterNonPlayer::StartDissolveTimeline()
{
    // 모든 머티리얼을 동적 인스턴스로 교체 (개별적으로 녹기 위함)
    const int32 MaterialCount = GetMesh()->GetNumMaterials();
    for (int32 i = 0; i < MaterialCount; ++i)
    {
        GetMesh()->CreateDynamicMaterialInstance(i);
    }

    // 디졸브 값 초기화
    CurrentDissolveValue = 0.0f;

    // 0.02초마다 업데이트 (부드러운 연출)
    GetWorld()->GetTimerManager().SetTimer(DissolveTimelineTimerHandle, this, &APPCharacterNonPlayer::UpdateDissolveMaterial, 0.02f, true);
}

void APPCharacterNonPlayer::UpdateDissolveMaterial()
{
    // DeadEventDelayTime 동안 0에서 1로 변하도록 속도 계산
    // 예: DelayTime이 5초면, 1초에 0.2씩 증가해야 함 (1.0 / 5.0 = 0.2)
    DissolveSpeed = (DeadEventDelayTime > 0.0f) ? (1.0f / DeadEventDelayTime) : 1.0f;

    // 0.02초 간격이므로 그만큼 보정
    CurrentDissolveValue += (0.02f * DissolveSpeed);

    if (CurrentDissolveValue >= 1.0f)
    {
        CurrentDissolveValue = 1.0f;
        GetWorld()->GetTimerManager().ClearTimer(DissolveTimelineTimerHandle);

        // (선택) 완전히 투명해지면 메쉬 숨기기
        GetMesh()->SetVisibility(false);
    }

    // 머티리얼 파라미터 업데이트 (이름: "Dissolve")
    GetMesh()->SetScalarParameterValueOnMaterials(FName("Dissolve"), CurrentDissolveValue);
}
