// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/MS_Kraken.h"


// Sets default values
AMS_Kraken::AMS_Kraken()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMS_Kraken::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMS_Kraken::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMS_Kraken::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AMS_Kraken::GetAIPatrolRadius()
{
	return 800.0f;
}

float AMS_Kraken::GetAIDetectRange()
{
	return 1000.0f;
}

float AMS_Kraken::GetAIAttackRange()
{
	return 100.0f; // ���� ������Ʈ�� ���� ������ ����� �Է�
}

float AMS_Kraken::GetAITurnSpeed()
{
	return 2.0f;
}

void AMS_Kraken::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AMS_Kraken::AttackByAI()
{
	// ������ ������ �ٸ�
	// ĳ���� ���̽����� ���� ���� �� ����
}

