
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PPHUD.generated.h"

UCLASS()
class PROPJECTPTEST_API APPHUD : public AHUD
{
    GENERATED_BODY()

public:
    APPHUD();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    void ShowBossHealthBar(AActor* BossActor);
    void HideBossHealthBar(AActor* BossActor);

    // 외부에서 만든 위젯을 HUD 관리 목록(CurrentWidgets)에 등록하는 함수
    void RegisterWidgetToHUD(UUserWidget* InWidget);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetHUDVisibility(bool bVisible);

	UPROPERTY()
	TObjectPtr<class UPPGASHpBarUserWidget> BossHpBarWidget;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TArray<TSubclassOf<class UUserWidget>> HUDWidgetClasses;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TArray <TSubclassOf<class UPPGASHpBarUserWidget>> GASWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> ExitWidgetClass;

    UPROPERTY()
    TArray<TObjectPtr<class UUserWidget>> CurrentWidgets;

    // --------------------------------------

    UPROPERTY()
    TObjectPtr<class UUserWidget> ExitWidget;

    bool bIsVisible;

    // --------------------------------------

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UUserWidget> MinimapWidgetClass;

    UPROPERTY()
    TObjectPtr<class UMinimapWidget> MinimapWidget;

    UPROPERTY(EditAnywhere, Category = "Minimap")
    FVector VolumeOrigin;

    UPROPERTY(EditAnywhere, Category = "Minimap")
    FVector VolumeExtent;

    UPROPERTY(EditAnywhere, Category = "Minimap")
    FVector2D MinimapSize;

    FVector PlayerLocation;

    bool bIsMinimapVisible;

    void ToggleMinimap();
};
