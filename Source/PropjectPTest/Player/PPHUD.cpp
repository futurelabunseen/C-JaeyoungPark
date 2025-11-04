
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

    // ◀◀◀ 1. 소유 플레이어 컨트롤러를 가져옵니다.
    APlayerController* PlayerController = GetOwningPlayerController();

    // ◀◀◀ 2. 이 HUD가 로컬 플레이어의 것인지 확인합니다. (매우 중요!)
    // 이 HUD가 서버에 있거나, 다른 클라이언트의 HUD가 (잘못)복제된 것이라면
    // 위젯을 생성하면 안 됩니다.
    if (!PlayerController || !PlayerController->IsLocalPlayerController())
    {
        return; // 로컬 플레이어의 HUD가 아니므로 아무것도 하지 않습니다.
    }

    FString CurrentLevelName = GetWorld()->GetMapName();
    CurrentLevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

    for (TSubclassOf<UUserWidget> WidgetClass : HUDWidgetClasses)
    {
        if (WidgetClass)
        {
            // ◀◀◀ 3. CreateWidget에 GetWorld() 대신 PlayerController를 전달합니다.
            UUserWidget* Widget = CreateWidget<UUserWidget>(PlayerController, WidgetClass);
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
            // ◀◀◀ 3. CreateWidget에 GetWorld() 대신 PlayerController를 전달합니다.
            BossHpBarWidget = CreateWidget<UPPGASHpBarUserWidget>(PlayerController, GASWidget);
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
            // ◀◀◀ 3. CreateWidget에 GetWorld() 대신 PlayerController를 전달합니다.
            ExitWidget = CreateWidget<UUserWidget>(PlayerController, ExitWidgetClass);
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
        // ◀◀◀ 3. CreateWidget에 GetWorld() 대신 PlayerController를 전달합니다.
        UUserWidget* Widget = CreateWidget<UUserWidget>(PlayerController, MinimapWidgetClass);
        if (Widget)
        {
            MinimapWidget = Cast<UMinimapWidget>(Widget);
            if (MinimapWidget)
            {
                MinimapWidget->AddToViewport();
                MinimapWidget->SetVisibility(ESlateVisibility::Hidden); // 초기 상태는 숨김
                MinimapWidget->InitializeMinimap(VolumeOrigin, VolumeExtent, MinimapSize);

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

    // ◀◀◀ 4. GetFirstPlayerController() 대신 안전하게 PlayerController 변수를 사용합니다.
    if (PlayerController && PlayerController->InputComponent)
    {
        PlayerController->InputComponent->BindAction("ToggleMinimap", IE_Pressed, this, &APPHUD::ToggleMinimap);
    }
}


void APPHUD::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ◀◀◀ 1. GetFirstPlayerController() 대신 GetOwningPlayerController()를 사용합니다.
    APlayerController* PlayerController = GetOwningPlayerController();

    // ◀◀◀ 2. IsLocalPlayerController() 체크를 여기서도 수행합니다 (안전장치).
    if (IsValid(MinimapWidget) && bIsMinimapVisible && PlayerController && PlayerController->IsLocalPlayerController())
    {
        // ◀◀◀ 3. GetPlayerCharacter(0) 대신 PlayerController->GetPawn()을 사용합니다.
        ACharacter* PlayerCharacter = PlayerController->GetPawn<ACharacter>();
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
