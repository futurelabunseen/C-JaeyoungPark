// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemSpawner.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItemSpawner::AItemSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    SpawnerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

// Called when the game starts or when spawned
void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AItemSpawner::SpawnItem, SpawnInterval, true);

    // 초기 아이템 스폰
    SpawnItem();
}

// Called every frame
//void AItemSpawner::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}

void AItemSpawner::SpawnItem()
{
    if (InteractableItemClass && HasAuthority()) // 서버에서만 실행되도록 체크
    {
        const UStaticMeshSocket* ResultSocket = SpawnerMesh->GetSocketByName(FName("HealItemLocation")); // 소켓 가져오기

        if (ResultSocket && !IsItemPresent()) // 소켓이 있고 아이템이 존재하지 않는다면
        {
            // 소켓의 트랜스폼을 가져와서 위치와 회전 분리
            FTransform SocketTransform;
            FTransform SocketAbsoluteTransform;
            if (ResultSocket->GetSocketTransform(SocketAbsoluteTransform, SpawnerMesh))
            {
                SocketTransform = SocketAbsoluteTransform;
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to get socket transform"));
                return;
            }

            FVector SocketLocation = SocketTransform.GetLocation();
            FRotator SocketRotation = SocketTransform.GetRotation().Rotator();

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;

            // 아이템 스폰
            AActor* SpawnedItem = GetWorld()->SpawnActor<AActor>(InteractableItemClass, SocketLocation, SocketRotation, SpawnParams);

            //if (SpawnedItem)
            //{
            //    // 스폰된 아이템을 소켓에 부착
            //    SpawnedItem->AttachToComponent(SpawnerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("HealItemLocation"));
            //}
        }
    }
    // UE_LOG(LogTemp, Error, TEXT("Excute Success Spawn!"));
}

bool AItemSpawner::IsItemPresent() const
{
    // 현재 월드에서 해당 아이템의 인스턴스를 찾음
    TArray<AActor*> FoundItems;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), InteractableItemClass, FoundItems);

    // 아이템이 존재하는지 여부를 반환
    return FoundItems.Num() > 0;
}