
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/PPGASHpBarUserWidget.h"
#include "UI/PPGASPlayerStatusUserWidget.h"
#include "PPHUD.generated.h"

UCLASS()
class PROPJECTPTEST_API APPHUD : public AHUD
{
    GENERATED_BODY()

public:
    APPHUD();

    virtual void BeginPlay() override;
    void ShowBossHealthBar(AActor* BossActor);
    void ShowStatus(AActor* PlayerActor);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetHUDVisibility(bool bVisible);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TArray<TSubclassOf<class UUserWidget>> HUDWidgetClasses;

    UPROPERTY()
    TArray<TObjectPtr<class UUserWidget>> CurrentWidgets;

    // --------------------------------------

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TArray <TSubclassOf<class UPPGASHpBarUserWidget>> GASWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TArray <TSubclassOf<class UPPGASPlayerStatusUserWidget>> GASPlayerWidgetClass;

    UPROPERTY()
    TObjectPtr<class UPPGASHpBarUserWidget> BossHpBarWidget;

    UPROPERTY()
    TObjectPtr<class UPPGASPlayerStatusUserWidget> PlayerStatusUserWidget;

    bool bIsVisible;
};
