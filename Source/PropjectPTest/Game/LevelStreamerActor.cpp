#include "LevelStreamerActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"

ALevelStreamerActor::ALevelStreamerActor()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelStreamerActor::OnOverlapBegin);

    // 기본값 설정
    DungeonLevelName = "ElvenRuins_Dungeon";
    BossLevelName = "ElvenRuins_Boss";
    BossStartLocation = FVector(3235.0f, 110.0f, 8637.0f);
}

void ALevelStreamerActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority()) return;

    // 이미 로딩 중이라면 무시
    if (bIsLoading) return;

    if (OtherActor && OtherActor->ActorHasTag("Player"))
    {
        bIsLoading = true; // 로딩 시작 표시

        // 트리거 비활성화 (더 이상 체크 안 함)
        TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        FLatentActionInfo LoadLatentInfo;
        LoadLatentInfo.CallbackTarget = this;
        LoadLatentInfo.ExecutionFunction = FName("OnBossLevelLoaded");
        LoadLatentInfo.Linkage = 0;
        LoadLatentInfo.UUID = 1;

        UGameplayStatics::LoadStreamLevel(this, BossLevelName, false, true, LoadLatentInfo);
    }
}

void ALevelStreamerActor::OnBossLevelLoaded()
{
    FLatentActionInfo ShowLatentInfo;
    ShowLatentInfo.CallbackTarget = this;
    ShowLatentInfo.ExecutionFunction = FName("OnBossLevelVisible");
    ShowLatentInfo.Linkage = 0;
    ShowLatentInfo.UUID = 2;

    UGameplayStatics::LoadStreamLevel(this, BossLevelName, true, true, ShowLatentInfo);
}

void ALevelStreamerActor::OnBossLevelVisible()
{
    MoveAllPlayersToBossRoom();

    GetWorld()->GetTimerManager().SetTimer(UnloadTimerHandle, this, &ALevelStreamerActor::DelayedUnloadDungeon, 3.0f, false);
}

void ALevelStreamerActor::DelayedUnloadDungeon()
{
    FLatentActionInfo UnloadLatentInfo;
    UnloadLatentInfo.CallbackTarget = this;
    UnloadLatentInfo.ExecutionFunction = FName("OnDungeonUnloaded");
    UnloadLatentInfo.Linkage = 0;
    UnloadLatentInfo.UUID = 3;

    UGameplayStatics::UnloadStreamLevel(this, DungeonLevelName, UnloadLatentInfo, false);
}

void ALevelStreamerActor::OnDungeonUnloaded()
{
    // 로딩 로직 종료, 필요하다면 변수 초기화
    bIsLoading = false;
}

void ALevelStreamerActor::MoveAllPlayersToBossRoom()
{
    UWorld* World = GetWorld();
    if (!World) return;

    int32 PlayerIndex = 0;
    float Spacing = 150.0f;

    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (PC && PC->GetPawn())
        {
            APawn* Pawn = PC->GetPawn();
            FVector SpawnOffset = FVector(0.0f, PlayerIndex * Spacing, 0.0f);
            FVector FinalLocation = BossStartLocation + SpawnOffset;

            Pawn->SetActorLocation(FinalLocation, false, nullptr, ETeleportType::TeleportPhysics);

            // 시선(회전)도 같이 맞춰주기
            FRotator BossViewRotation = FRotator(0.0f, 90.0f, 0.0f);
            PC->SetControlRotation(BossViewRotation);

            PlayerIndex++;
        }
    }
}