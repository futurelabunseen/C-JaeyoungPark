#include "Character/PPCharacterNonPlayer.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"

APPCharacterNonPlayer::APPCharacterNonPlayer()
{
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void APPCharacterNonPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void APPCharacterNonPlayer::SetDead()
{
    // 부모의 SetDead 호출 
    // -> 여기서 IsDeadFlag = true가 되고, 서버는 즉시 OnRep_IsDeadFlag 호출됨.
    // -> 클라이언트는 복제 후 OnRep_IsDeadFlag 호출됨.
    Super::SetDead();

    // AI 정리 (서버에서만 실행하면 됨)
    if (HasAuthority())
    {
        AAIController* AIController = Cast<AAIController>(GetController());
        if (IsValid(AIController))
        {
            AIController->StopMovement();
            AIController->UnPossess();
        }

        // 액터 파괴 예약 (서버 권한)
        float AnimLength = 1.5f;
        if (GetDeadMontage())
        {
            AnimLength = GetDeadMontage()->GetPlayLength() - 0.1f;
        }
        float TotalDestroyDelay = AnimLength + DeadEventDelayTime;

        GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
            [&]()
            {
                Destroy();
            }
        ), TotalDestroyDelay, false);
    }
}

void APPCharacterNonPlayer::OnRep_IsDeadFlag()
{
    // 부모의 기본 로직 실행 (애니메이션 재생, 콜리전 끄기 등)
    Super::OnRep_IsDeadFlag();

    // 만약 죽은 상태라면 -> 디졸브 스케줄링 시작
    if (IsDeadFlag)
    {
        float AnimLength = 1.5f;
        if (GetDeadMontage())
        {
            AnimLength = GetDeadMontage()->GetPlayLength() - 0.1f;
        }

        // "애니메이션이 끝날 때쯤" 디졸브를 시작하기 위해 로컬 타이머 설정
        // 이 코드는 서버(Listen Server)와 모든 클라이언트에서 각자 실행됨
        GetWorld()->GetTimerManager().SetTimer(DissolveTimerHandle, this, &APPCharacterNonPlayer::StartDissolveTimeline, AnimLength, false);
    }
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

        // 완전히 투명해지면 메쉬 숨기기
        GetMesh()->SetVisibility(false);
    }

    // 머티리얼 파라미터 업데이트 (이름: "Dissolve")
    // 이제 이 값에 따라 머티리얼 내부에서 1-x 계산과 Step 분기를 통해 스스로 불타는 효과와 투명화를 동시에 처리합니다.
    GetMesh()->SetScalarParameterValueOnMaterials(FName("Dissolve"), CurrentDissolveValue);
}