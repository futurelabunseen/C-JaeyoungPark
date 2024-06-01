
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/PPGASHpBarUserWidget.h"
#include "PPHUD.generated.h"

UCLASS()
class PROPJECTPTEST_API APPHUD : public AHUD
{
    GENERATED_BODY()

public:
    APPHUD();

    void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
    void ShowBossHealthBar(AActor* BossActor);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TArray<TSubclassOf<class UUserWidget>> HUDWidgetClasses;

    UPROPERTY()
    TArray<TObjectPtr<class UUserWidget>> CurrentWidgets;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UPPGASHpBarUserWidget> GASWidgetClass;

    UPROPERTY()
    TObjectPtr<class UPPGASHpBarUserWidget> BossHpBarWidget;
};
