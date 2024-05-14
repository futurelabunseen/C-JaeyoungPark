// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "PPGASInteractionItem.generated.h"

UCLASS()
class PROPJECTPTESTGAS_API APPGASInteractionItem : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APPGASInteractionItem();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void NotifyActorBeginOverlap(class AActor* Other) override;
	virtual void NotifyActorEndOverlap(class AActor* Other) override;

protected:
	virtual void PostInitializeComponents() override;

	void ApplyEffectToTarget(AActor* Target);
	void InvokeGameplayCue(AActor* Target);

protected:
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, Category = Bound)
	TObjectPtr<class USphereComponent> SphereTrigger;

	UPROPERTY(VisibleAnywhere, Category = Bound)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = Widget)
	TObjectPtr<class UWidgetComponent> Widget;

	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = GAS, Meta = (Categories = GameplayCue))
	FGameplayTag GameplayCueTag;
};
