
#include "PPHUD.h"
#include "Blueprint/UserWidget.h"
#include "Monster/Boss_Mermaid.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

APPHUD::APPHUD()
{
    PrimaryActorTick.bCanEverTick = true;
    // BossHpBar = CreateDefaultSubobject<UPPGASWidgetComponent>(TEXT("BossWidget"));
    
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
}

//void APPHUD::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    ABoss_Mermaid* Boss = Cast<ABoss_Mermaid>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss_Mermaid::StaticClass()));
//    if (Boss)
//    {
//        if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
//        {
//            APawn* PlayerPawn = PlayerController->GetPawn();
//            if (PlayerPawn)
//            {
//                FVector BossLocation = Boss->GetActorLocation();
//                FVector PlayerLocation = PlayerPawn->GetActorLocation();
//                float Distance = FVector::Dist(BossLocation, PlayerLocation);
//
//                if (Distance <= Boss->GetAIDetectRange())
//                {
//                    if (!BossHpBarWidget)
//                    {
//                        BossHpBarWidget = CreateWidget<UPPGASWidgetComponent>(GetWorld(), BossHpBarWidget);
//                        if (BossHpBarWidget)
//                        {
//                            BossHpBarWidget->AddToViewport();
//                        }
//                    }
//                }
//                else
//                {
//                    if (BossHpBarWidget)
//                    {
//                        BossHpBarWidget->RemoveFromViewport();
//                        BossHpBarWidget = nullptr;
//                    }
//                }
//            }
//        }
//    }
//}

