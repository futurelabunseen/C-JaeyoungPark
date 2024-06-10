
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
	TArray<UMaterialInterface*> OriginalMaterials; // ���� ��Ƽ���� �迭

	FTimerHandle OverlayTimerHandle; // �������� Ÿ�̸� �ڵ�
	FTimerHandle DamageTextTimerHandle; // ������ �ؽ�Ʈ Ÿ�̸� �ڵ�

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
