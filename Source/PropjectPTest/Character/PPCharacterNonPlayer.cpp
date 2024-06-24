
#include "Character/PPCharacterNonPlayer.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Engine/World.h"

APPCharacterNonPlayer::APPCharacterNonPlayer()
{
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void APPCharacterNonPlayer::SetDead()
{
    Super::SetDead();

    // AI 중지 코드 추가
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->StopMovement();
        AIController->UnPossess();
    }

    // FTimerHandle DeadTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
        [&]()
        {
            Destroy();
        }
    ), DeadEventDelayTime, false);
}
