// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PPCharacter.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "PPGASCharacter.generated.h"

/**
 *
 */
UCLASS()
class PROPJECTPTESTGAS_API APPGASCharacter : public APPCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	APPGASCharacter();

	FORCEINLINE virtual class UAnimMontage* GetSkillActionMontage() const { return SkillActionMontage; }
	FORCEINLINE virtual class UAnimMontage* GetDashActionMontage() const { return DashActionMontage; }

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 멀티플레이에서는 서버에서만 호출되는 형태가 된다. 만약 네트웍 멀티플레이에서 이것을 전달 받으려면 OnLapPlayerState 이벤트 함수에서 구현해줘야 함
	virtual void PossessedBy(AController* NewController) override;

	//플레이어 입력 정의
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void SetupGASInputComponent();
	void GASInputPressed(int32 InputId);
	void GASInputReleased(int32 InputId);

	UFUNCTION()
	virtual void OnOutOfHealth();

	// virtual void NotifyComboActionEnd();

protected:

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, Category = GAS)
	TMap<int32, TSubclassOf<class UGameplayAbility>> StartInputAbilities;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UPPGASWidgetComponent> HpBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> SkillActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> DashActionMontage;
};
