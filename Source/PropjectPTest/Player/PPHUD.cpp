
#include "PPHUD.h"
#include "Blueprint/UserWidget.h"
#include "Monster/Boss_Mermaid.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UI/PPGASHpBarUserWidget.h"
#include "UI/PPGASPlayerStatusUserWidget.h"
#include "Character/PPGASCharacter.h"

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
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create HUD widget."));
            }
        }
    }

    for (TSubclassOf<UUserWidget> GASWidget : GASWidgetClass)
    {
        if (GASWidget)
        {
            BossHpBarWidget = CreateWidget<UPPGASHpBarUserWidget>(GetWorld(), GASWidget);
            if (BossHpBarWidget)
            {
                BossHpBarWidget->AddToViewport();
                BossHpBarWidget->SetVisibility(ESlateVisibility::Hidden);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create BossHpBarWidget."));
            }
        }
    }

    for (TSubclassOf<UUserWidget> GASPlayerWidget : GASPlayerWidgetClass)
    {
        if (GASPlayerWidget)
        {
            PlayerStatusUserWidget = CreateWidget<UPPGASPlayerStatusUserWidget>(GetWorld(), GASPlayerWidget);
            if (PlayerStatusUserWidget)
            {
                // APPGASCharacter* Player = Cast<APPGASCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
                PlayerStatusUserWidget->AddToViewport();
                CurrentWidgets.Add(PlayerStatusUserWidget);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create PlayerStatusUserWidget."));
            }
        }
    }
}

void APPHUD::ShowBossHealthBar(AActor* BossActor)
{
    if (BossHpBarWidget)
    {
        BossHpBarWidget->SetVisibility(ESlateVisibility::Visible);
        BossHpBarWidget->SetAbilitySystemComponent(BossActor);
    }
}

void APPHUD::ShowStatus(AActor* PlayerActor)
{
    if (PlayerActor)
    {
        PlayerStatusUserWidget->SetAbilitySystemComponent(PlayerActor);
        // PlayerStatusUserWidget->AddToViewport();
    }
}

void APPHUD::SetHUDVisibility(bool bVisible)
{
    ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

    for (UUserWidget* Widget : CurrentWidgets)
    {
        if (Widget)
        {
            Widget->SetVisibility(NewVisibility);
        }
    }
}
