// PPHUD.cpp

#include "PPHUD.h"
#include "Blueprint/UserWidget.h"
#include "Monster/Boss_Mermaid.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UI/PPGASHpBarUserWidget.h"

APPHUD::APPHUD()
{
    PrimaryActorTick.bCanEverTick = true;
}

void APPHUD::BeginPlay()
{
    Super::BeginPlay();

    for (TSubclassOf<UUserWidget> WidgetClass : HUDWidgetClasses)
    {
        if (WidgetClass)
        {
            UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
            if (Widget)
            {
                Widget->AddToViewport();
                CurrentWidgets.Add(Widget);
            }
        }
    }

    if (GASWidgetClass)
    {
        BossHpBarWidget = CreateWidget<UPPGASHpBarUserWidget>(GetWorld(), GASWidgetClass);
        if (BossHpBarWidget)
        {
            BossHpBarWidget->AddToViewport();
            BossHpBarWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void APPHUD::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APPHUD::ShowBossHealthBar(AActor* BossActor)
{
    if (BossHpBarWidget)
    {
        BossHpBarWidget->SetVisibility(ESlateVisibility::Visible);
        BossHpBarWidget->SetAbilitySystemComponent(BossActor);
    }
}
