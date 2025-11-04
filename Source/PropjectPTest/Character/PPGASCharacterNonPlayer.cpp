// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PPGASCharacterNonPlayer.h"
#include "AbilitySystemComponent.h"
#include "Monster/Boss_Mermaid.h"
#include "Player/PPHUD.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AI/MS/MSAIController.h"


APPGASCharacterNonPlayer::APPGASCharacterNonPlayer()
{
    PrimaryActorTick.bCanEverTick = false;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);

    InitialState = EAIState::Active;
    CurrentAIState = EAIState::Active;
}

UAbilitySystemComponent* APPGASCharacterNonPlayer::GetAbilitySystemComponent() const
{
	return ASC;
}

void APPGASCharacterNonPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ASC->InitAbilityActorInfo(this, this);

    SetAIState(InitialState);
	// UE_LOG(LogTemp, Error, TEXT("%s Monster"), *ASC->GetOwner()->GetName());

	const FGameplayTag EnemyTag = FGameplayTag::RequestGameplayTag(FName("Character.Enemy"));
	ASC->AddLooseGameplayTag(EnemyTag);

	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(InitStatEffect, Level, EffectContextHandle);
	if (EffectSpecHandle.IsValid())
	{
		ASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
	}

	for (const auto& StartAbility : StartAbilities)
	{
		FGameplayAbilitySpec StartSpec(StartAbility);
		ASC->GiveAbility(StartSpec);
	}
}

void APPGASCharacterNonPlayer::OnOutOfHealth()
{
	SetDead();
}

void APPGASCharacterNonPlayer::SetAIState(EAIState NewState)
{
    if (CurrentAIState == NewState) return;
    CurrentAIState = NewState;

    // CharacterMovementComponent와 Mesh 가져오기
    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    USkeletalMeshComponent* SkeletalMesh = GetMesh();

    if (AMSAIController* MSAIController = Cast<AMSAIController>(GetController())) // ◀◀◀ AAIController 대신 AMSAIController로 Cast
    {
        // BrainComponent는 AAIController에도 있지만, RunAI()는 AMSAIController에 정의되어 있습니다.
        // 그래서 AMSAIController로 캐스팅해야 합니다.

        switch (CurrentAIState)
        {
        case EAIState::Active:
            MSAIController->RunAI(); // ◀◀◀ BrainComponent->StartLogic() 대신 AIController의 RunAI() 호출
            if (MovementComponent)
            {
                MovementComponent->SetComponentTickEnabled(true);
                MovementComponent->SetMovementMode(MOVE_Walking);
            }
            if (SkeletalMesh)
            {
                SkeletalMesh->bPauseAnims = false;
                SkeletalMesh->SetComponentTickEnabled(true);
            }
            UE_LOG(LogTemp, Warning, TEXT("%s state changed to ACTIVE"), *GetName());
            break;

        case EAIState::Relevant:
            MSAIController->StopAI(); // ◀◀◀ StopAI() 호출
            if (MovementComponent)
            {
                MovementComponent->StopMovementImmediately();
                MovementComponent->SetComponentTickEnabled(false);
            }
            if (SkeletalMesh)
            {
                SkeletalMesh->bPauseAnims = true;
                SkeletalMesh->SetComponentTickEnabled(false);
            }
            UE_LOG(LogTemp, Log, TEXT("%s state changed to RELEVANT"), *GetName());
            break;

        case EAIState::Dormant:
            MSAIController->StopAI(); // ◀◀◀ StopAI() 호출
            if (MovementComponent)
            {
                MovementComponent->StopMovementImmediately();
                MovementComponent->SetComponentTickEnabled(false);
            }
            if (SkeletalMesh)
            {
                SkeletalMesh->bPauseAnims = true;
                SkeletalMesh->SetComponentTickEnabled(false);
            }
            UE_LOG(LogTemp, Warning, TEXT("%s state changed to DORMANT"), *GetName());
            break;
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("SetAIState: Failed to cast Controller to AMSAIController for %s"), *GetName());
    }
}
