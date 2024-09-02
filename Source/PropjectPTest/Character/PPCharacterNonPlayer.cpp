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

    // ���̾ư��� ����Ʈ�� �ε��� �� �ֵ��� �غ�
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

    // AI ���� �ڵ� �߰�
    AAIController* AIController = Cast<AAIController>(GetController());
    if (IsValid(AIController))
    {
        AIController->StopMovement();
        AIController->UnPossess();
    }

    if (HasAuthority()) // ���������� ���̾ư��� ȿ�� ����
    {
        // Ÿ�̸Ӹ� �����Ͽ� ���� �ð� �Ŀ� ���̾ư��� ȿ�� ����
        // GetWorld()->GetTimerManager().SetTimer(DissolveTimerHandle, this, &APPCharacterNonPlayer::TriggerDissolveEffect, DeadEventDelayTime, false);

        TriggerDissolveEffect();

        // ���� �ð� �Ŀ� ��ü ����
        GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
            [&]()
            {
                Destroy();
            }
        ), DeadEventDelayTime, false); // ȿ���� ���� �� ���� �� ���� �Ŀ� ����
    }
}

void APPCharacterNonPlayer::TriggerDissolveEffect()
{
    if (HasAuthority()) // ���������� ���̾ư��� ȿ�� ����
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

    // ���⿡ �ʿ��� ������ �߰��� ����� �� �ֽ��ϴ�.
}
