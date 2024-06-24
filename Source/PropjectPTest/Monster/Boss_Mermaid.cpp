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


// Sets default values
ABoss_Mermaid::ABoss_Mermaid()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Boss AI Setting
	AIControllerClass = ABossAIController::StaticClass();

	BossAttributeSet = CreateDefaultSubobject<UBossAttributeSet>(TEXT("BossAttributeSet"));

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(GetMesh());
	DetectionSphere->SetSphereRadius(1000.0f); // Set the detection radius
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss_Mermaid::OnOverlapBegin);

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
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, this, &ABoss_Mermaid::DisconnectFromServer, 5.0f, false);
}

void ABoss_Mermaid::DisconnectFromServer()
{
	// 서버 연결 끊기 로직 구현
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->ClientTravel("/Game/Maps/Demonstration_Village.Demonstration_Village'", TRAVEL_Absolute);
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

void ABoss_Mermaid::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void ABoss_Mermaid::AttackByAI()
{
	//PlayAnimMontage(AttackMontage);
	// 예제와 구조가 다름
	// 캐릭터 베이스에서 구조 변경 후 적용

}

void ABoss_Mermaid::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		// OtherActor가 플레이어 캐릭터인지 확인합니다.
		APawn* PlayerPawn = Cast<APawn>(OtherActor);
		if (PlayerPawn)
		{
			APlayerController* PlayerController = Cast<APlayerController>(PlayerPawn->GetController());
			if (PlayerController)
			{
				APPHUD* PlayerHUD = Cast<APPHUD>(PlayerController->GetHUD());
				if (PlayerHUD)
				{
					PlayerHUD->ShowBossHealthBar(this);
					
					// Server에서 DetectionSphere 반지름 줄이기 호출
					if (HasAuthority())
					{
						ReduceDetectionRadiusMulticastRPC();
					}
				}
			}
		}
	}
}

void ABoss_Mermaid::ReduceDetectionRadiusMulticastRPC_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("MulticastReduceDetectionRadius called on clients"));
	// 모든 클라이언트에서 반지름 줄이기
	DetectionSphere->SetSphereRadius(100.0f);
	DetectionSphere->UpdateOverlaps();
}

