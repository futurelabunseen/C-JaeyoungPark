// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "PPGASInteractionItem.generated.h"

UCLASS()
class PROPJECTPTEST_API APPGASInteractionItem : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APPGASInteractionItem();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void NotifyActorBeginOverlap(class AActor* Other) override;
	virtual void NotifyActorEndOverlap(class AActor* Other) override;

	UPROPERTY(EditAnywhere, Category = Bound)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
	TObjectPtr<class UParticleSystem> ParticleSystem;

	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* InteractionSound;

protected:
	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> TargetCharacterASC;


	UPROPERTY(VisibleAnywhere, Category = Bound)
	TObjectPtr<class USphereComponent> SphereTrigger;

	UPROPERTY(VisibleAnywhere, Category = Widget)
	TObjectPtr<class UWidgetComponent> Widget;
};
