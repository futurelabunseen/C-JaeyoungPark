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
	FORCEINLINE virtual class UAnimMontage* GetComboActionMontage() const { return ComboActionMontage; }
	FORCEINLINE class UPPComboActionData* GetComboActionData() const { return ComboActionData; }

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	void ZoomIn();
	void ZoomOut();

	//플레이어 입력 정의
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 멀티플레이에서는 서버에서만 호출되는 형태가 된다. 만약 네트웍 멀티플레이에서 이것을 전달 받으려면 OnLapPlayerState 이벤트 함수에서 구현해줘야 함
	virtual void PossessedBy(AController* NewController) override;

protected:
	void SetupGASInputComponent();
	void GASInputPressed(int32 InputId);
	void GASInputReleased(int32 InputId);

	UFUNCTION()
	virtual void OnOutOfHealth();

	virtual void Tick(float DeltaTime) override;

	// virtual void NotifyComboActionEnd();

protected:

	// GAS Section
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, Category = GAS)
	TMap<int32, TSubclassOf<class UGameplayAbility>> StartInputAbilities;

	// --------------------------------------------------------------

	// Character Section
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UPPGASWidgetComponent> HpBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> SkillActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> DashActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPPComboActionData> ComboActionData;

	// --------------------------------------------------------------

	// Action Input Section
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ZoomInAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ZoomOutAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DashAction;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	//스프링암 길이 목표값
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SpringArm, meta = (AllowPrivateAccess = "true"))
	float ExpectedSpringArmLength = 800.0f;
};
