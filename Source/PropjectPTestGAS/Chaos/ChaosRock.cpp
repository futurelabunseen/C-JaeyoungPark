// Fill out your copyright notice in the Description page of Project Settings.


#include "Chaos/ChaosRock.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/BoxComponent.h"
#include "Character/PPGASCharacter.h"

// Sets default values
AChaosRock::AChaosRock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	RockMesh = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("RockMesh"));
	SetRootComponent(RockMesh);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RockMesh);
	BoxCollision->SetGenerateOverlapEvents(true);
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

