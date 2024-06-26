// Fill out your copyright notice in the Description page of Project Settings.


#include "Chaos/ChaosRock.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/BoxComponent.h"
#include "Character/PPGASCharacter.h"

// Sets default values
AChaosRock::AChaosRock()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RockMesh = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("RockMesh"));
	SetRootComponent(RockMesh);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RockMesh);
	BoxCollision->SetGenerateOverlapEvents(true);

	DestructionSound = CreateDefaultSubobject<UAudioComponent>(TEXT("DestructionSound"));
	DestructionSound->SetupAttachment(RockMesh);
	DestructionSound->bAutoActivate = false;

	FadeOutDuration = 5.0f;  // 소리가 점점 작아지는 시간 (초)
	InitialVolume = 1.0f;    // 초기 볼륨
}

// Called when the game starts or when spawned
void AChaosRock::BeginPlay()
{
	Super::BeginPlay();
	RockMesh->SetSimulatePhysics(false);
	// BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AChaosRock::OnOverlap);
	// BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AChaosRock::EndOverlap);
}

void AChaosRock::NotifyActorBeginOverlap(AActor* Other)
{
	const auto player = Cast<APPGASCharacter>(Other);
	if (player)
	{
		RockMesh->SetSimulatePhysics(true);
		SetLifeSpan(10.0f);

		if (DestructionSoundCue)
		{
			DestructionSound->SetSound(DestructionSoundCue);
			DestructionSound->Play();
			GetWorldTimerManager().SetTimer(FadeOutTimerHandle, this, &AChaosRock::FadeOutSound, 0.1f, true);
		}
	}
}

void AChaosRock::FadeOutSound()
{
	float CurrentVolume = DestructionSound->VolumeMultiplier;
	float NewVolume = FMath::Max(CurrentVolume - (InitialVolume / (FadeOutDuration * 10)), 0.0f);

	if (NewVolume <= 0.0f)
	{
		DestructionSound->Stop();
		GetWorldTimerManager().ClearTimer(FadeOutTimerHandle);
	}
	else
	{
		DestructionSound->SetVolumeMultiplier(NewVolume);
	}
}

//void AChaosRock::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor)
//	{
//		const auto player = Cast<APPGASCharacter>(OtherActor);
//		if (player)
//		{
//			RockMesh->SetSimulatePhysics(true);
//			// RockDestroyDele.ExecuteIfBound(GetActorLocation());
//			// BoxCollision->DestroyComponent();
//		}
//	}
//}
//
//void AChaosRock::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	
//}