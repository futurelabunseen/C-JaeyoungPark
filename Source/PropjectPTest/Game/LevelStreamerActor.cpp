#include "LevelStreamerActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"

ALevelStreamerActor::ALevelStreamerActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // 멤버 변수에 바로 할당
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelStreamerActor::OnOverlapBegin);

    DungeonLevelName = "ElvenRuins_Dungeon";
    BossLevelName = "ElvenRuins_Boss";
    BossStartLocation = FVector(3235.0f, 110.0f, 8637.0f);
}

void ALevelStreamerActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority()) return; // 서버에서만 실행

    if (OtherActor && OtherActor->ActorHasTag("Player"))
    {
        // 보스 레벨 로드 완료 후 콜백으로 던전 언로드
        FLatentActionInfo LoadLatentInfo;
        LoadLatentInfo.CallbackTarget = this;
        LoadLatentInfo.ExecutionFunction = FName("OnBossLevelLoaded");
        LoadLatentInfo.Linkage = 0;
        LoadLatentInfo.UUID = 1;

        UGameplayStatics::LoadStreamLevel(this, BossLevelName, true, true, LoadLatentInfo);
    }
}

// 보스 레벨 로드 완료 후 호출
void ALevelStreamerActor::OnBossLevelLoaded()
{
    // 던전 레벨 언로드
    FLatentActionInfo UnloadLatentInfo;
    UnloadLatentInfo.CallbackTarget = this;
    UnloadLatentInfo.ExecutionFunction = NAME_None;
    UnloadLatentInfo.Linkage = 0;
    UnloadLatentInfo.UUID = 2;

    UGameplayStatics::UnloadStreamLevel(this, DungeonLevelName, UnloadLatentInfo, false);

    // 모든 플레이어 이동
    MulticastMovePlayers();
}

// 모든 플레이어 이동
void ALevelStreamerActor::MulticastMovePlayers_Implementation()
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (PC)
        {
            APawn* Pawn = PC->GetPawn();
            if (Pawn)
            {
                Pawn->SetActorLocation(BossStartLocation);
            }
        }
    }
}
