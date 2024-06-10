
#pragma once

#include "CoreMinimal.h"
#include "UI/PPGASUserWidget.h"
#include "GameplayEffectTypes.h"
#include "PPGASDamageTextUserWidget.generated.h"

/**
 *
 */
UCLASS()
class PROPJECTPTEST_API UPPGASDamageTextUserWidget : public UPPGASUserWidget
{
	GENERATED_BODY()
public:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

protected:
	virtual void SpawnDamage(const FOnAttributeChangeData& ChangeData);

	// Override NativeConstruct
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DamageTextBlock;

private:
	UPROPERTY()
	AActor* OwnerActor;
};
