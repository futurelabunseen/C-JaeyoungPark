#include "LevelStreamerActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Engine/LevelStreaming.h"

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

        UGameplayStatics::LoadStreamLevel(this, BossLevelName, false, true, LoadLatentInfo);
    }
}

// 2. 메모리 로드 완료 -> 이제 "화면에 띄우기" 명령 시작
void ALevelStreamerActor::OnBossLevelLoaded()
{
    // 여기서 바로 이동하면 추락합니다!
    // 대신, "화면에 띄우는 작업"이 끝날 때까지 기다리는 LatentInfo를 하나 더 만듭니다.

    FLatentActionInfo ShowLatentInfo;
    ShowLatentInfo.CallbackTarget = this;
    ShowLatentInfo.ExecutionFunction = FName("OnBossLevelVisible"); // 2단계 콜백 (새로 추가)
    ShowLatentInfo.Linkage = 0;
    ShowLatentInfo.UUID = 2; // UUID 변경

    // 이미 로딩된 레벨에 대해 LoadStreamLevel을 다시 호출합니다.
    // 이번에는 bMakeVisibleAfterLoad = true 입니다.
    // 엔진은 로딩은 생략하고 "Visible 처리"가 끝나는 시점에 ShowLatentInfo를 실행해줍니다.
    UGameplayStatics::LoadStreamLevel(this, BossLevelName, true, true, ShowLatentInfo);
}

// 3. 화면 가시화 및 콜리전 생성 완료 -> 안전하게 이동
void ALevelStreamerActor::OnBossLevelVisible()
{
    // 이제 땅바닥이 확실히 존재합니다. 플레이어를 이동시킵니다.
    MulticastMovePlayers();
    GetWorld()->GetTimerManager().SetTimer(UnloadTimerHandle, this, &ALevelStreamerActor::DelayedUnloadDungeon, 3.0f, false);
}

void ALevelStreamerActor::OnLevelUnloaded()
{
}

void ALevelStreamerActor::DelayedUnloadDungeon()
{
    FLatentActionInfo UnloadLatentInfo;
    UnloadLatentInfo.CallbackTarget = this;
    UnloadLatentInfo.ExecutionFunction = FName("OnDungeonUnloaded"); // [로그 해결] 실제 함수 연결
    UnloadLatentInfo.Linkage = 0;
    UnloadLatentInfo.UUID = 3;

    // 이제 언로드 수행
    UGameplayStatics::UnloadStreamLevel(this, DungeonLevelName, UnloadLatentInfo, false);
}

void ALevelStreamerActor::OnDungeonUnloaded()
{
    // 언로드가 완전히 끝났습니다.
    // 로그 경고는 사라졌을 것입니다.

    // [선택 사항: GC 강제 수행]
    // 언로드가 끝나면 메모리에 "쓰레기(Pending Kill)"가 많이 쌓입니다.
    // 다음 자동 GC 때 렉이 걸릴 수 있으니, 
    // 만약 지금이 컷신 중이거나 전투 시작 전이라면 여기서 강제로 치워버리는 게 낫습니다.
    // GEngine->ForceGarbageCollection(true); 

    // UE_LOG(LogTemp, Log, TEXT("Dungeon Unloaded Cleanly!"));
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
