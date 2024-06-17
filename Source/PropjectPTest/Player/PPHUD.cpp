
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

    // 현재 레벨 이름 가져오기
    FString CurrentLevelName = GetWorld()->GetMapName();
    CurrentLevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

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

    if (CurrentLevelName == TEXT("ElvenRuins"))
    {
        if (ExitWidgetClass)
        {
            ExitWidget = CreateWidget<UUserWidget>(GetWorld(), ExitWidgetClass);
            if (ExitWidget)
            {
                ExitWidget->AddToViewport();
                CurrentWidgets.Add(ExitWidget);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create ExitWidget."));
                CurrentWidgets.Remove(ExitWidget);
            }
        }
    }

    /*for (TSubclassOf<UUserWidget> GASPlayerWidget : GASPlayerWidgetClass)
    {
        if (GASPlayerWidget)
        {
            PlayerStatusUserWidget = CreateWidget<UPPGASPlayerStatusUserWidget>(GetWorld(), GASPlayerWidget);
            if (PlayerStatusUserWidget)
            {
                PlayerStatusUserWidget->AddToViewport();
                CurrentWidgets.Add(PlayerStatusUserWidget);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create PlayerStatusUserWidget."));
            }
        }
    }*/
}

void APPHUD::ShowBossHealthBar(AActor* BossActor)
{
    if (BossHpBarWidget)
    {
        BossHpBarWidget->SetVisibility(ESlateVisibility::Visible);
        BossHpBarWidget->SetAbilitySystemComponent(BossActor);
    }
}

//void APPHUD::ShowStatus(AActor* PlayerActor)
//{
//    if (PlayerActor && PlayerStatusUserWidget)
//    {
//        PlayerStatusUserWidget->SetAbilitySystemComponent(PlayerActor);
//        // PlayerStatusUserWidget->AddToViewport();
//    }
//}

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
