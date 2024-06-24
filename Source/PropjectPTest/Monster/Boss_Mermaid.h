// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/BossAIInterface.h"
#include "Character/PPGASCharacterNonPlayer.h"
#include "Boss_Mermaid.generated.h"

UCLASS()
class PROPJECTPTEST_API ABoss_Mermaid : public APPGASCharacterNonPlayer, public IBossAIInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss_Mermaid();

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnOutOfHealth() override;

	void DisconnectFromServer();

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Detection")
	TObjectPtr<class USphereComponent> DetectionSphere;*/

	// AI Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> AttackMontage;

	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;

	// Multicast to sync the detection sphere radius on all clients
	/*UFUNCTION(NetMulticast, Unreliable)
	void ReduceDetectionRadiusMulticastRPC();*/

	// void Tick(float DeltaTime) override;

	FAICharacterAttackFinished OnAttackFinished;

	// virtual void NotifyComboActionEnd() override;

	/*UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);*/
};
