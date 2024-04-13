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

	// ��Ƽ�÷��̿����� ���������� ȣ��Ǵ� ���°� �ȴ�. ���� ��Ʈ�� ��Ƽ�÷��̿��� �̰��� ���� �������� OnLapPlayerState �̺�Ʈ �Լ����� ��������� ��
	virtual void PossessedBy(AController* NewController) override;

	//�÷��̾� �Է� ����
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
