#include "Character/PPCharacterNonPlayer.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"

// 불필요해진 나이아가라 헤더들은 제거했습니다.
// #include "NiagaraFunctionLibrary.h" 
// #include "NiagaraComponent.h"
// #include "UObject/ConstructorHelpers.h" (만약 다른 에셋 로드에 안 쓴다면 제거 가능)

APPCharacterNonPlayer::APPCharacterNonPlayer()
{
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // [삭제됨] 나이아가라 이펙트 로드 부분
    // 이제 머티리얼이 스스로 빛나므로 별도의 파티클 에셋을 로드할 필요가 없습니다.
}

void APPCharacterNonPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void APPCharacterNonPlayer::SetDead()
{
    // 1. 상위 클래스(APPCharacter)의 SetDead 호출
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
        float AnimLength = 1.5f;
        if (GetDeadMontage())
        {
            AnimLength = GetDeadMontage()->GetPlayLength() - 0.1f;
        }

        // [타이머 1] 애니메이션 끝날 때쯤 -> 디졸브 시작 (Multicast)
        GetWorld()->GetTimerManager().SetTimer(DissolveTimerHandle, this, &APPCharacterNonPlayer::Multicast_StartDissolve, AnimLength, false);

        // [타이머 2] 파괴 예약
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
    // [삭제됨] 나이아가라 이펙트 스폰 부분
    // 이제 파티클을 소환하지 않고, 바로 몸체를 녹이는 함수만 호출합니다.

    // 1. 몬스터 몸체(Mesh) 녹이기 시작
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
    // 이제 이 값에 따라 머티리얼 내부에서 1-x 계산과 Step 분기를 통해
    // 스스로 불타는 효과와 투명화를 동시에 처리합니다.
    GetMesh()->SetScalarParameterValueOnMaterials(FName("Dissolve"), CurrentDissolveValue);
}