// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "PPAT_JumpAndWaitForLanding.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpAndWaitForLandingDelegate);

/**
 * 
 */
UCLASS()
class PROPJECTPTESTGAS_API UPPAT_JumpAndWaitForLanding : public UAbilityTask
{
	GENERATED_BODY()
public:
	UPPAT_JumpAndWaitForLanding();


	// UFUNCTION 옵션 : 블루프린트에서 사용가능, 어빌리티의 태스크 카테고리, 보여지는 이름, 숨길 파라미터?, 기본값?, 내부에서 블루프린트 사용 여부
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "JumpAndWaitForLanding",
		HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UPPAT_JumpAndWaitForLanding* CreateTask(UGameplayAbility* OwningAbility);

	// 활성화 정의
	virtual void Activate() override;

	// 파괴 정의
	virtual void OnDestroy(bool AbilityEnded) override;

	// 델리게이트 정의
	UPROPERTY(BlueprintAssignable)
	FJumpAndWaitForLandingDelegate OnComplete;

protected:
	// 바닥 닿기 정의
	UFUNCTION()
	void OnLandedCallback(const FHitResult& Hit);



};
