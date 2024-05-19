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

    // �ʱ� ������ ����
    SpawnItem();
}

// Called every frame
//void AItemSpawner::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}

void AItemSpawner::SpawnItem()
{
    if (InteractableItemClass && HasAuthority()) // ���������� ����ǵ��� üũ
    {
        const UStaticMeshSocket* ResultSocket = SpawnerMesh->GetSocketByName(FName("HealItemLocation")); // ���� ��������

        if (ResultSocket && !IsItemPresent()) // ������ �ְ� �������� �������� �ʴ´ٸ�
        {
            // ������ Ʈ�������� �����ͼ� ��ġ�� ȸ�� �и�
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

            // ������ ����
            AActor* SpawnedItem = GetWorld()->SpawnActor<AActor>(InteractableItemClass, SocketLocation, SocketRotation, SpawnParams);

            //if (SpawnedItem)
            //{
            //    // ������ �������� ���Ͽ� ����
            //    SpawnedItem->AttachToComponent(SpawnerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("HealItemLocation"));
            //}
        }
    }
    // UE_LOG(LogTemp, Error, TEXT("Excute Success Spawn!"));
}

bool AItemSpawner::IsItemPresent() const
{
    // ���� ���忡�� �ش� �������� �ν��Ͻ��� ã��
    TArray<AActor*> FoundItems;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), InteractableItemClass, FoundItems);

    // �������� �����ϴ��� ���θ� ��ȯ
    return FoundItems.Num() > 0;
}