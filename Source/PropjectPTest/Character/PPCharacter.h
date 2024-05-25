

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

	FORCEINLINE class UAnimMontage* GetDeadMontage() const { return DeadMontage; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;

// Dead Section
protected:
	virtual void BeginPlay() override;

	virtual void SetDead();

	UFUNCTION(NetMulticast, Unreliable)
	void DeadMulticastRPC();

	void PlayDeadAnimation();

	float DeadEventDelayTime = 5.0f;
};

