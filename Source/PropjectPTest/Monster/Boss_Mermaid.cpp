// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Boss_Mermaid.h"
#include "AI/Boss/BossAIController.h"
#include "Attribute/BossAttributeSet.h"
#include "Character/PPGASCharacter.h"
#include "Player/PPHUD.h"
#include "Components/SphereComponent.h"
#include "UI/PPGASWidgetComponent.h"
#include "UI/PPGASUserWidget.h"
#include "TimerManager.h"
#include "Player/PPPlayerController.h"
#include "Physics/PPCollision.h"
#include "Net/UnrealNetwork.h"


ABoss_Mermaid::ABoss_Mermaid()
{
	PrimaryActorTick.bCanEverTick = true;

	// Boss AI Setting
	AIControllerClass = ABossAIController::StaticClass();

	BossAttributeSet = CreateDefaultSubobject<UBossAttributeSet>(TEXT("BossAttributeSet"));

	DamageText = CreateDefaultSubobject<UPPGASWidgetComponent>(TEXT("DamageTextWidget"));
	DamageText->SetupAttachment(GetMesh());
	static ConstructorHelpers::FClassFinder<UUserWidget> DamageTextWidgetRef(TEXT("/Game/UI/WBP_DamageText.WBP_DamageText_C"));
	if (DamageTextWidgetRef.Class)
	{
		DamageText->SetWidgetClass(DamageTextWidgetRef.Class);
		DamageText->SetWidgetSpace(EWidgetSpace::Screen);
		DamageText->SetDrawSize(FVector2D(200.0f, 50.0f));
		DamageText->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ABoss_Mermaid::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	BossAttributeSet->OnOutOfHealth_Boss.AddDynamic(this, &ABoss_Mermaid::OnOutOfHealth);
}

void ABoss_Mermaid::OnOutOfHealth()
{
	Super::OnOutOfHealth();

	// 5초 후에 서버 연결을 끊는 함수 호출
	if (HasAuthority())
	{
		MulticastHidePlayerHUDsRPC();
		FTimerHandle DeadMonsterTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DeadMonsterTimerHandle, this, &ABoss_Mermaid::DisconnectFromServer, 3.0f, false);
		UE_LOG(LogTemp, Warning, TEXT("OnOutOfHealth: Timer set for DisconnectFromServer"));
	}
}

void ABoss_Mermaid::DisconnectFromServer()
{
	UE_LOG(LogTemp, Warning, TEXT("DisconnectFromServer: Called"));

	UWorld* World = GetWorld();
	if (World)
	{

		// 모든 플레이어 컨트롤러를 순회하면서 레벨을 변경합니다.
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (IsValid(PC))
			{
				// 새로운 맵의 이름을 설정합니다.
				FName NewLevelName = TEXT("Demonstration_Village"); // 원하는 맵 이름으로 변경

				// 모든 클라이언트를 새로운 맵으로 이동시킵니다.
				PC->ClientTravel(NewLevelName.ToString(), TRAVEL_Absolute);
				UE_LOG(LogTemp, Warning, TEXT("DisconnectFromServer: ClientTravel called for PC %s"), *PC->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("DisconnectFromServer: Invalid PlayerController found"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DisconnectFromServer: World is not valid"));
	}
}

void ABoss_Mermaid::MulticastHidePlayerHUDsRPC_Implementation()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (IsValid(PC))
		{
			APPHUD* PlayerHUD = Cast<APPHUD>(PC->GetHUD());
			if (IsValid(PlayerHUD))
			{
				// 모든 클라이언트에서 HUD 가시성 변경
				PlayerHUD->BossHpBarWidget->SetVisibility(ESlateVisibility::Hidden);
				UE_LOG(LogTemp, Warning, TEXT("MulticastHidePlayerHUDsRPC: Hidden BossHpBarWidget for PC %s"), *PC->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("MulticastHidePlayerHUDsRPC: Invalid PlayerHUD for PC %s"), *PC->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MulticastHidePlayerHUDsRPC: Invalid PlayerController found"));
		}
	}
}


float ABoss_Mermaid::GetAIPatrolRadius()
{
	return 800.0f; // 어트리뷰트 세트로 변경 예정
}

float ABoss_Mermaid::GetAIDetectRange()
{
	return 3000.0f; // 어트리뷰트 세트로 변경 예정
}

float ABoss_Mermaid::GetAIAttackRange()
{
	return BossAttributeSet->GetAttackRange();
}

float ABoss_Mermaid::GetAITurnSpeed()
{
	return 2.0f; // 어트리뷰트 세트로 변경 예정
}
