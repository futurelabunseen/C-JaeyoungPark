// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PPCharacter.generated.h"

UCLASS(Blueprintable)
class PROPJECTPTEST_API APPCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APPCharacter();

	/*FORCEINLINE virtual class UAnimMontage* GetComboActionMontage() const { return ComboActionMontage; }
	FORCEINLINE class UPPComboActionData* GetComboActionData() const { return ComboActionData; }*/

	FORCEINLINE class UAnimMontage* GetDeadMontage() const { return DeadMontage; }

	// FORCEINLINE class USkeletalMesh* GetCharacterMesh() const { return CharacterMesh; }
	// FORCEINLINE class UAnimInstance* GetAnimInstanceClass() const { return AnimInstanceClass; }
	
	// FORCEINLINE class UAnimMontage* GetAttackMontage() const { return AttackMontage; }

	///** Returns TopDownCameraComponent subobject **/
	//FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	///** Returns CameraBoom subobject **/
	//FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	//void ZoomIn();
	//void ZoomOut();
	////플레이어 입력 정의
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

protected:

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPPComboActionData> ComboActionData;*/

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Base, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMesh> CharacterMesh;*/

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Base, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UAnimInstance> AnimInstanceClass;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> AttackMontage;*/

	// --------------------------------------------------------

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
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
	TObjectPtr<class UInputAction> DashAction;*/

// Dead Section
protected:
	virtual void BeginPlay() override;

	void Tick(float DeltaTime);

	virtual void SetDead();

	void PlayDeadAnimation();

	float DeadEventDelayTime = 5.0f;

//private:
//	/** Top down camera */
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
//	class UCameraComponent* TopDownCameraComponent;
//
//	/** Camera boom positioning the camera above the character */
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
//	class USpringArmComponent* CameraBoom;
//
//	//스프링암 길이 목표값
//	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SpringArm, Meta = (AllowPrivateAccess = true))
//	float ExpectedSpringArmLength = 800.0f;
};

