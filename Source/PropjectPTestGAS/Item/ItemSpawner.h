// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "PPGASInteractionItem.h"
#include "ItemSpawner.generated.h"

UCLASS()
class PROPJECTPTESTGAS_API AItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSpawner();

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

protected:
	
	void SpawnItem();

	FTimerHandle SpawnTimerHandle;
	bool IsItemPresent() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> SpawnerMesh; // ���� ����ƽ �޽�

	UPROPERTY(EditAnywhere, Category = Item)
	TSubclassOf<class APPGASInteractionItem> InteractableItemClass; // ��ȣ�ۿ� ������ Ŭ����

	UPROPERTY(EditAnywhere)
	float SpawnInterval = 10.0f; // ���� ����
};
