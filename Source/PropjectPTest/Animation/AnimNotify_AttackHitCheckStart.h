
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "AnimNotify_AttackHitCheckStart.generated.h"

UCLASS()
class PROPJECTPTEST_API UAnimNotify_AttackHitCheckStart : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_AttackHitCheckStart();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FGameplayTag TriggerGameplayTag;

private:
	void StopTimer(AActor* OwnerActor);

	static bool bTimerSet; // Timer flag to prevent multiple timers
	FTimerHandle TimerHandle; // Timer handle for managing the timer
};