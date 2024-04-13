

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

	FORCEINLINE class UAnimMontage* GetDeadMontage() const { return DeadMontage; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;

// Dead Section
protected:
	virtual void BeginPlay() override;

	void Tick(float DeltaTime);

	virtual void SetDead();

	void PlayDeadAnimation();

	float DeadEventDelayTime = 5.0f;
};

