
#include "Game/LevelStreamerActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Engine/LevelStreaming.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerStart.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"

ALevelStreamerActor::ALevelStreamerActor()
{
    PrimaryActorTick.bCanEverTick = true;

    OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
    RootComponent = OverlapVolume;

    OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALevelStreamerActor::OverlapBegins);
}

void ALevelStreamerActor::BeginPlay()
{
    Super::BeginPlay();
}

void ALevelStreamerActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void ALevelStreamerActor::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (OtherActor == MyCharacter && LevelToLoad != "")
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlap detected with player character"));

        // Load the new level
        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget = this;
        LatentInfo.ExecutionFunction = "OnLevelLoaded";
        LatentInfo.Linkage = 0;
        LatentInfo.UUID = FMath::Rand();

        UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentInfo);

        // Unload the old level
        /*if (LevelToUnLoad != "")
        {
            UGameplayStatics::UnloadStreamLevel(this, LevelToUnLoad, LatentInfo, false);
        }*/
    }
}

void ALevelStreamerActor::OnLevelLoaded()
{
    UE_LOG(LogTemp, Warning, TEXT("Level loaded"));
    MovePlayerToStart();
}

void ALevelStreamerActor::MovePlayerToStart()
{
    ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (MyCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player character is valid"));

        // Ensure LevelToLoad is a FName
        ULevelStreaming* LoadedLevel = UGameplayStatics::GetStreamingLevel(this, LevelToLoad);
        if (LoadedLevel && LoadedLevel->IsLevelLoaded())
        {
            ULevel* Level = LoadedLevel->GetLoadedLevel();
            if (Level)
            {
                UE_LOG(LogTemp, Warning, TEXT("Level is valid"));
                FName TargetTag = "BossStart";

                for (AActor* Actor : Level->Actors)
                {
                    if (Actor)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Checking Actor: %s"), *Actor->GetName());
                        APlayerStart* PlayerStart = Cast<APlayerStart>(Actor);
                        if (PlayerStart)
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Found PlayerStart: %s"), *PlayerStart->GetName());
                            if (PlayerStart->ActorHasTag(TargetTag))
                            {
                                UE_LOG(LogTemp, Warning, TEXT("PlayerStart has target tag: %s"), *PlayerStart->GetName());
                                MyCharacter->TeleportTo(PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
                                return;
                            }
                            else
                            {
                                UE_LOG(LogTemp, Warning, TEXT("PlayerStart does not have target tag"));
                            }
                        }
                    }
                }
                UE_LOG(LogTemp, Warning, TEXT("No PlayerStart with the specified tag found in the level"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Loaded level is null"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Loaded level is not loaded or invalid"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Player character is null"));
    }
}
