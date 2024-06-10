
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TObjectPtr<class UMaterialInterface> OverlayMaterial;

	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials; // 원래 머티리얼 배열

	FTimerHandle OverlayTimerHandle; // 오버레이 타이머 핸들
	FTimerHandle DamageTextTimerHandle; // 데미지 텍스트 타이머 핸들

	UFUNCTION()
	void ApplyOverlayMaterial();

	UFUNCTION()
	void ResetMaterial();

	UFUNCTION()
	void ShowDamageText(float Damage);

private:
	UPROPERTY()
	TObjectPtr <class AActor> OwnerActor;
};
