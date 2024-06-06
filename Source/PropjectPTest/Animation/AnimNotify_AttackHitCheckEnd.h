
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "AnimNotify_AttackHitCheckEnd.generated.h"

UCLASS()
class PROPJECTPTEST_API UAnimNotify_AttackHitCheckEnd : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_AttackHitCheckEnd();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
