

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "PPCharacter.generated.h"

UCLASS(Blueprintable)
class PROPJECTPTEST_API APPCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APPCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE class UAnimMontage* GetDeadMontage() const { return DeadMontage; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;

	UPROPERTY(ReplicatedUsing = OnRep_IsDeadFlag, BlueprintReadOnly, Category = "State")
	bool IsDeadFlag = false;

	UFUNCTION()
	void OnRep_IsDeadFlag();

// Dead Section
protected:

	virtual void SetDead();
	void PlayDeadAnimation();
	float DeadEventDelayTime = 5.0f;
};

