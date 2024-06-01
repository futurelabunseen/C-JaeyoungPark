// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Boss_Mermaid.h"
#include "AI/Boss/BossAIController.h"
#include "Attribute/BossAttributeSet.h"
#include "Character/PPGASCharacter.h"
#include "Player/PPHUD.h"
#include "Components/SphereComponent.h"


// Sets default values
ABoss_Mermaid::ABoss_Mermaid()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Boss AI Setting
	AIControllerClass = ABossAIController::StaticClass();

	BossAttributeSet = CreateDefaultSubobject<UBossAttributeSet>(TEXT("BossAttributeSet"));

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->SetSphereRadius(1000.0f); // Set the detection radius
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoss_Mermaid::OnOverlapBegin);
}

void ABoss_Mermaid::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	BossAttributeSet->OnOutOfHealth_Boss.AddDynamic(this, &ThisClass::OnOutOfHealth);
}

float ABoss_Mermaid::GetAIPatrolRadius()
{
	return 800.0f; // 어트리뷰트 세트로 변경 예정
}

float ABoss_Mermaid::GetAIDetectRange()
{
	return 1000.0f; // 어트리뷰트 세트로 변경 예정
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
				}
			}
		}
	}
}
