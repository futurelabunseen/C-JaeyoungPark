
#include "PPHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UI/PPGASHpBarUserWidget.h"
#include "UI/PPGASPlayerStatusUserWidget.h"
#include "Character/PPGASCharacter.h"
#include "Misc/OutputDeviceNull.h"
#include "GameFramework/PlayerController.h"
#include "UI/MinimapWidget.h"


APPHUD::APPHUD()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsMinimapVisible = false;

    VolumeOrigin = FVector(1520.0f, 7270.0f, 8180.0f);
    VolumeExtent = FVector(20000.0f, 20000.0f, 10000.0f);
    MinimapSize = FVector2D(1920.0f, 1080.0f);
}

void APPHUD::BeginPlay()
{
    Super::BeginPlay();

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

    if (MinimapWidgetClass)
    {
        UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), MinimapWidgetClass);
        if (Widget)
        {
            MinimapWidget = Cast<UMinimapWidget>(Widget);
            if (MinimapWidget)
            {
                MinimapWidget->AddToViewport();
                MinimapWidget->SetVisibility(ESlateVisibility::Hidden); // 초기 상태는 숨김
                MinimapWidget->InitializeMinimap(VolumeOrigin, VolumeExtent, MinimapSize);

                // MinimapWidget 크기 로그 추가
                FVector2D WidgetSize = MinimapWidget->GetDesiredSize();
                UE_LOG(LogTemp, Log, TEXT("MinimapWidget Size: X=%f, Y=%f"), WidgetSize.X, WidgetSize.Y);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to cast MinimapWidget to UMinimapWidget."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create Minimap widget."));
        }
    }

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->InputComponent->BindAction("ToggleMinimap", IE_Pressed, this, &APPHUD::ToggleMinimap);
    }

    //UE_LOG(LogTemp, Log, TEXT("Volume Origin: X=%f, Y=%f, Z=%f"), VolumeOrigin.X, VolumeOrigin.Y, VolumeOrigin.Z);
    //UE_LOG(LogTemp, Log, TEXT("Volume Extent: X=%f, Y=%f, Z=%f"), VolumeExtent.X, VolumeExtent.Y, VolumeExtent.Z);
    //UE_LOG(LogTemp, Log, TEXT("Minimap Size: X=%f, Y=%f"), MinimapSize.X, MinimapSize.Y);
}


void APPHUD::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (IsValid(MinimapWidget) && bIsMinimapVisible && PlayerController && PlayerController->IsLocalPlayerController())
    {
        ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        if (PlayerCharacter)
        {
            PlayerLocation = PlayerCharacter->GetActorLocation();
            MinimapWidget->UpdatePlayerIconPosition(PlayerLocation);
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

void APPHUD::HideBossHealthBar(AActor* BossActor)
{
    if (BossHpBarWidget)
    {
        BossHpBarWidget->SetVisibility(ESlateVisibility::Hidden);
        BossHpBarWidget->RemoveFromParent();
        UE_LOG(LogTemp, Warning, TEXT("HideBossHealthBar: BossHpBarWidget removed from parent!"));
        // BossHpBarWidget->SetAbilitySystemComponent(BossActor);
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

void APPHUD::ToggleMinimap()
{
    if (MinimapWidget)
    {
        bIsMinimapVisible = !bIsMinimapVisible;
        ESlateVisibility NewVisibility = bIsMinimapVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
        MinimapWidget->SetVisibility(NewVisibility);
    }
}
