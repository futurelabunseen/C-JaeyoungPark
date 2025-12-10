#include "ChaosRock.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Character/PPGASCharacter.h" 
#include "Chaos/ChaosCache.h" 
#include "GeometryCollection/GeometryCollectionCache.h"

AChaosRock::AChaosRock()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. 카오스 메시 (초기화)
    RockMesh = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("RockMesh"));
    SetRootComponent(RockMesh);
    RockMesh->SetSimulatePhysics(false);
    RockMesh->SetMobility(EComponentMobility::Movable);
    RockMesh->SetCollisionProfileName(TEXT("BlockAll"));

    // 2. 스태틱 메시 (초기 모습)
    RockStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RockStaticMesh"));
    RockStaticMesh->SetupAttachment(RockMesh);
    RockStaticMesh->SetCollisionProfileName(TEXT("BlockAll"));
    RockStaticMesh->SetMobility(EComponentMobility::Movable);

    // 3. 트리거
    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(RockMesh);
    BoxCollision->SetGenerateOverlapEvents(true);
    BoxCollision->SetCollisionProfileName(TEXT("Trigger"));

    // 4. 사운드
    DestructionSound = CreateDefaultSubobject<UAudioComponent>(TEXT("DestructionSound"));
    DestructionSound->SetupAttachment(RockMesh);
    DestructionSound->bAutoActivate = false;

    // 변수 초기화
    FadeOutDuration = 3.0f;
    bHasTriggered = false;
    RunningTime = 0.0f; // 시간 변수 초기화

    // CacheManager 기본 설정
    CacheMode = ECacheMode::Play;
    StartMode = EStartMode::Timed;
}

void AChaosRock::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // 1. 카오스 메쉬와 스태틱 메쉬 모빌리티 설정
    if (RockMesh) RockMesh->SetMobility(EComponentMobility::Movable);
    if (RockStaticMesh) RockStaticMesh->SetMobility(EComponentMobility::Movable);

    // 2. ★ 핵심: 관찰 대상 강제 설정 (에디터에 즉시 반영됨)
    if (RockMesh)
    {
        // 보호된 배열에 안전하게 접근하는 함수 사용
        FObservedComponent& Observed = FindOrAddObservedComponent(RockMesh);

        // 캐시 이름 설정 (컬렉션 저장용)
        Observed.CacheName = FName("CashingRock");

        // ★ 잘못된 레퍼런스(SM_Plains...) 강제 수정 로직
        // "이 컴포넌트의 주인은 나(Self)다" 라고 명시
        Observed.SoftComponentRef.PathToComponent = nullptr;

        // "내 컴포넌트 이름은 RockMesh다" 라고 명시
        Observed.SoftComponentRef.ComponentProperty = RockMesh->GetFName();

        Observed.bIsSimulating = true;
    }
}

void AChaosRock::BeginPlay()
{
    // 부모 BeginPlay 호출 (매우 중요)
    Super::BeginPlay();

    // ★ 녹화(Record) 모드일 때의 로직
    if (CacheMode == ECacheMode::Record)
    {
        UE_LOG(LogTemp, Warning, TEXT("!!! RECORDING MODE !!! - Physics Enabled"));

        // 1. 녹화 대상인 Chaos Mesh를 바로 보여주고 물리를 켭니다.
        if (RockMesh)
        {
            RockMesh->SetHiddenInGame(false);
            RockMesh->SetSimulatePhysics(true); // ★ 녹화하려면 물리가 켜져 있어야 함!
            RockMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

            // 등록 (필수)
            // ★ 컴포넌트 등록 및 이름 명시적 지정
            FObservedComponent& Observed = FindOrAddObservedComponent(RockMesh);

            // 1. 캐시 이름 (스크린샷에 있는 "CashingRock"으로 통일 추천)
            Observed.CacheName = FName("CashingRock");

            // 2. ★ 핵심: 컴포넌트 이름을 명시적으로 지정
            Observed.SoftComponentRef.ComponentProperty = RockMesh->GetFName();

            // 3. (확실하게 하기 위해) 타겟 액터를 '나 자신(Self)'으로 설정
            Observed.SoftComponentRef.PathToComponent = nullptr;

            Observed.bIsSimulating = true;
        }

        // 2. 스태틱 메시는 방해되므로 숨깁니다.
        if (RockStaticMesh)
        {
            RockStaticMesh->SetHiddenInGame(true);
            RockStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        // 3. 녹화는 시작하자마자 프레임을 캡처해야 하므로 Tick을 켭니다.
        SetActorTickEnabled(true);
    }
    // ★ 재생(Play) 모드일 때의 로직 (기존 코드)
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
            RockMesh->SetSimulatePhysics(false); // 재생할 땐 물리 끄기
            RockMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            FObservedComponent& Observed = FindOrAddObservedComponent(RockMesh);
            Observed.CacheName = FName("CashingRock");
            Observed.bIsSimulating = true;
        }

        // 재생 대기 상태이므로 Tick 끄기
        SetActorTickEnabled(false);
    }
}

void AChaosRock::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bHasTriggered && CacheMode == ECacheMode::Play)
    {
        // 1. 내가 원하는 재생 시간을 직접 흐르게 함
        RunningTime += DeltaTime;

        // 2. [핵심 트릭] 
        // Timed 모드는 (현재시간 - 시작시간)을 재생 위치로 잡습니다.
        // 우리는 재생 위치가 RunningTime이 되기를 원합니다.
        // 즉, (CurrentTime - StartTime) = RunningTime 이 되어야 합니다.
        // 따라서 StartTime = CurrentTime - RunningTime 으로 역산해서 계속 넣어줍니다.

        float CurrentWorldTime = GetWorld()->GetTimeSeconds();
        SetStartTime(CurrentWorldTime - RunningTime);

        // 디버깅용 로그 (재생 시간이 잘 흐르는지 확인)
        // UE_LOG(LogTemp, Log, TEXT("Playing Cache: %.2f sec"), RunningTime);
    }
}

void AChaosRock::NotifyActorBeginOverlap(AActor* Other)
{
    if (bHasTriggered) return;

    const auto Player = Cast<APPGASCharacter>(Other);
    if (!Player) return;

    TriggerDestruction();
}

void AChaosRock::TriggerDestruction()
{
    if (bHasTriggered) return;
    bHasTriggered = true;

    UE_LOG(LogTemp, Warning, TEXT("[%s] TriggerDestruction Called!"), *GetName());

    // 1. 메시 교체
    if (RockStaticMesh)
    {
        RockStaticMesh->SetHiddenInGame(true);
        RockStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (RockMesh)
    {
        RockMesh->SetHiddenInGame(false);
        RockMesh->SetSimulatePhysics(false);
        RockMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    if (CacheCollection)
    {
        CacheMode = ECacheMode::Play;
        StartMode = EStartMode::Timed;

        // ★ [핵심] 사용자의 요청대로 시작 시간을 0으로 초기화
        RunningTime = 0.0f;

        // 초기 시작점 설정 (0초부터 시작)
        //SetStartTime(RunningTime);

        float CurrentWorldTime = GetWorld()->GetTimeSeconds();
        SetStartTime(CurrentWorldTime);

        // ★ Tick을 켜서 Tick 함수 내부의 '더하기 로직'이 돌아가게 함
        PrimaryActorTick.bCanEverTick = true;
        SetActorTickEnabled(true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] CacheCollection is NULL!"), *GetName());
    }

    // 3. 사운드 재생
    if (DestructionSound && DestructionSoundCue)
    {
        DestructionSound->SetSound(DestructionSoundCue);
        DestructionSound->Play();
        DestructionSound->FadeOut(FadeOutDuration, 0.0f);
    }

    // 4. 액터 수명 설정
    SetLifeSpan(10.0f);
}