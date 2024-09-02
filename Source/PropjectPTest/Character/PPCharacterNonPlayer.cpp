#include "Character/PPCharacterNonPlayer.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"
#include "Net/UnrealNetwork.h" // For networking functions

APPCharacterNonPlayer::APPCharacterNonPlayer()
{
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // 나이아가라 이펙트를 로드할 수 있도록 준비
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> DissolveEffectFinder(TEXT("/Script/Niagara.NiagaraSystem'/Game/Effects/DissolveEffect.DissolveEffect'"));
    if (DissolveEffectFinder.Succeeded())
    {
        DissolveEffect = DissolveEffectFinder.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Niagara Effect is Not Set"));
    }
}

void APPCharacterNonPlayer::SetDead()
{
    Super::SetDead();

    // AI 중지 코드 추가
    AAIController* AIController = Cast<AAIController>(GetController());
    if (IsValid(AIController))
    {
        AIController->StopMovement();
        AIController->UnPossess();
    }

    if (HasAuthority()) // 서버에서만 나이아가라 효과 실행
    {
        // 타이머를 설정하여 일정 시간 후에 나이아가라 효과 실행
        // GetWorld()->GetTimerManager().SetTimer(DissolveTimerHandle, this, &APPCharacterNonPlayer::TriggerDissolveEffect, DeadEventDelayTime, false);

        TriggerDissolveEffect();

        // 일정 시간 후에 객체 제거
        GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
            [&]()
            {
                Destroy();
            }
        ), DeadEventDelayTime, false); // 효과를 적용 후 조금 더 지연 후에 제거
    }
}

void APPCharacterNonPlayer::TriggerDissolveEffect()
{
    if (HasAuthority()) // 서버에서만 나이아가라 효과 실행
    {
        if (DissolveEffect)
        {
            UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
                DissolveEffect,
                GetMesh(),
                NAME_None,
                FVector::ZeroVector,
                FRotator::ZeroRotator,
                EAttachLocation::KeepRelativeOffset,
                true);

            if (NiagaraComponent)
            {
                NiagaraComponent->Activate();
            }
        }
    }
}

void APPCharacterNonPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 여기에 필요한 변수를 추가로 등록할 수 있습니다.
}
