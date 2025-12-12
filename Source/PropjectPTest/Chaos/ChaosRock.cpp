#include "ChaosRock.h"
#include "Character/PPGASCharacter.h" 
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "LevelSequenceActor.h" 
#include "LevelSequencePlayer.h"
#include "Sound/SoundBase.h"

AChaosRock::AChaosRock()
{
    PrimaryActorTick.bStartWithTickEnabled = false;

    // 1. 컴포넌트 초기화
    RockMesh = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("RockMesh"));
    SetRootComponent(RockMesh);
    RockMesh->SetSimulatePhysics(false);
    RockMesh->SetMobility(EComponentMobility::Movable);
    RockMesh->SetCollisionProfileName(TEXT("BlockAll"));

    RockStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RockStaticMesh"));
    RockStaticMesh->SetupAttachment(RockMesh);
    RockStaticMesh->SetCollisionProfileName(TEXT("BlockAll"));
    RockStaticMesh->SetMobility(EComponentMobility::Movable);

    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(RockMesh);
    BoxCollision->SetGenerateOverlapEvents(true);
    BoxCollision->SetCollisionProfileName(TEXT("Trigger"));

    DestructionSound = CreateDefaultSubobject<UAudioComponent>(TEXT("DestructionSound"));
    DestructionSound->SetupAttachment(RockMesh);
    DestructionSound->bAutoActivate = false;

    // 변수 초기화
    FadeOutDuration = 3.0f;
    bHasTriggered = false;

    CacheMode = ECacheMode::None;
    StartMode = EStartMode::Triggered;
}

void AChaosRock::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (RockMesh)
    {
        FObservedComponent& Observed = FindOrAddObservedComponent(RockMesh);
        Observed.CacheName = FName("CashingRock");
        Observed.SoftComponentRef.PathToComponent = nullptr;
        Observed.SoftComponentRef.ComponentProperty = RockMesh->GetFName();
    }
}

void AChaosRock::BeginPlay()
{
    Super::BeginPlay();

    // ★ 녹화(Record) 모드일 때만 Tick을 켬
    if (CacheMode == ECacheMode::Record)
    {
        if (RockMesh)
        {
            RockMesh->SetHiddenInGame(false);
            RockMesh->SetSimulatePhysics(true);
            RockMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }
        if (RockStaticMesh)
        {
            RockStaticMesh->SetHiddenInGame(true);
            RockStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        SetActorTickEnabled(true);
    }
    // ★ 재생(Play) 모드일 때는 Tick을 확실하게 꺼둠 (대기)
    else
    {
        if (RockStaticMesh)
        {
            RockStaticMesh->SetHiddenInGame(false);
            RockStaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }
        if (RockMesh)
        {
            RockMesh->SetHiddenInGame(true);
            RockMesh->SetSimulatePhysics(false);
            RockMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        SetActorTickEnabled(false);
    }
}

void AChaosRock::NotifyActorBeginOverlap(AActor* Other)
{
    if (bHasTriggered) return;

    // 캐릭터만 감지
    const auto Player = Cast<APPGASCharacter>(Other);
    if (!Player) return;

    TriggerDestruction();
}

void AChaosRock::TriggerDestruction()
{
    if (bHasTriggered) return;
    bHasTriggered = true;

    // 1. 메시 교체
    if (RockStaticMesh)
    {
        RockStaticMesh->SetHiddenInGame(true);
        RockStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (RockMesh)
    {
        RockMesh->SetHiddenInGame(false);
        RockMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    // 3. 사운드 재생
    if (DestructionSound && DestructionSoundCue)
    {
        DestructionSound->SetSound(DestructionSoundCue);
        DestructionSound->Play();
        DestructionSound->FadeOut(FadeOutDuration, 0.0f);
    }

    // 4. 레벨 시퀀스 재생
    if (LinkedSequenceActor) // 레벨에서 지정한 액터가 있다면
    {
        if (LinkedSequenceActor->GetSequencePlayer())
        {
            LinkedSequenceActor->GetSequencePlayer()->Play();
            LinkedSequenceActor->GetSequencePlayer()->OnFinished.AddDynamic(this, &AChaosRock::OnSequenceFinished);
        }
    }
    else
    {
        SetLifeSpan(10.0f);
    }
}

void AChaosRock::OnSequenceFinished()
{
    Destroy();
}