#include "InterestManager.h"
#include "Kismet/GameplayStatics.h"
#include "Character/PPGASCharacterNonPlayer.h"
#include "Monster/MS_Golem.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h" // APlayerController 헤더 추가
#include "Net/UnrealNetwork.h" // GetLifetimeReplicatedProps를 위해 추가

AInterestManager::AInterestManager()
{
    PrimaryActorTick.bCanEverTick = true;
    SetReplicates(true); // 서버에서 클라이언트로 리플리케이트되도록 설정 (액터의 존재 여부 등)
    // 몬스터 상태 업데이트는 서버에서만 수행되어야 하므로 Tick을 서버에서만 돌리도록 변경할 수 있음
    PrimaryActorTick.bStartWithTickEnabled = true; // 기본적으로 틱 활성화
    SetReplicatingMovement(false); // 이 액터는 움직임 복제가 필요 없습니다.
    bAlwaysRelevant = true;
}

// Net/UnrealNetwork.h 포함 후 이 함수 추가
void AInterestManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    // 몬스터 프록시는 서버에서만 관리하므로 리플리케이트할 필요 없음.
    // 만약 클라이언트도 몬스터 프록시 정보를 알아야 한다면 리플리케이트해야 함.
    // 여기서는 몬스터 액터 자체를 리플리케이트하는 것이 일반적이므로 프록시는 리플리케이트하지 않음.
}

void AInterestManager::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("AInterestManager::BeginPlay() called on %s"), HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"));

    // 서버에서만 매니저를 초기화하고 몬스터 상태 업데이트 타이머를 설정
    if (HasAuthority()) // ◀ 서버에서만 실행되도록 체크
    {
        GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &AInterestManager::InitializeManager, 0.5f, false);
    }
    else
    {
        // 클라이언트는 이 매니저 액터를 리플리케이트 받지만, 몬스터 상태 업데이트 로직은 서버에서만 처리
        // 클라이언트에서는 틱을 비활성화하여 불필요한 연산 방지
        SetActorTickEnabled(false);
    }
}

void AInterestManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 디버그 드로잉은 서버와 모든 클라이언트에서 각자 수행
    if (!bShowDebugSpheres || GetWorld()->IsGameWorld() == false)
    {
        return;
    }

    // 서버는 모든 플레이어를 순회하며 디버그 구를 그립니다.
    // 클라이언트는 자신의 플레이어 폰을 기준으로 디버그 구를 그립니다.
    if (HasAuthority())
    {
        // 서버는 모든 플레이어의 위치를 기준으로 디버그 그리기
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* PlayerController = It->Get();
            if (PlayerController && PlayerController->GetPawn())
            {
                DrawDebugInterestZones(PlayerController->GetPawn()->GetActorLocation());
            }
        }
    }
    else // 클라이언트
    {
        APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (LocalPlayerPawn)
        {
            DrawDebugInterestZones(LocalPlayerPawn->GetActorLocation());
        }
    }
}

// 디버그 드로잉 로직을 별도 함수로 분리
void AInterestManager::DrawDebugInterestZones(const FVector& Location)
{
    const FVector CylinderStart = Location - FVector(0.f, 0.f, VerticalTolerance);
    const FVector CylinderEnd = Location + FVector(0.f, 0.f, VerticalTolerance);

    DrawDebugCylinder(GetWorld(), CylinderStart, CylinderEnd, ActiveRadius, 32, FColor::Green, false, -1.0f, 0, 10.0f);
    DrawDebugCylinder(GetWorld(), CylinderStart, CylinderEnd, RelevantRadius, 32, FColor::Red, false, -1.0f, 0, 10.0f);
}


void AInterestManager::InitializeManager()
{
    if (!HasAuthority()) return; // 서버에서만 실행

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys)
    {
        UE_LOG(LogTemp, Error, TEXT("InterestManager: Navigation System not found! Initialization failed."));
        return;
    }

    TArray<AActor*> FoundMonsters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APPGASCharacterNonPlayer::StaticClass(), FoundMonsters);

    for (AActor* MonsterActor : FoundMonsters)
    {
        APPGASCharacterNonPlayer* Monster = Cast<APPGASCharacterNonPlayer>(MonsterActor);
        if (Monster)
        {
            FMonsterProxyData NewProxy;
            NewProxy.MonsterClass = Monster->GetClass();

            FNavLocation ProjectedLocation;
            if (NavSys->ProjectPointToNavigation(Monster->GetActorLocation(), ProjectedLocation, FVector(500.0f, 500.0f, 500.0f)))
            {
                NewProxy.LastKnownLocation = ProjectedLocation.Location;
            }
            else
            {
                NewProxy.LastKnownLocation = Monster->GetActorLocation();
                UE_LOG(LogTemp, Warning, TEXT("Could not project monster spawn point to NavMesh for monster at %s. Using original location."), *NewProxy.LastKnownLocation.ToString());
            }

            NewProxy.CurrentState = EAIState::Dormant;
            MonsterProxies.Add(NewProxy);
            Monster->Destroy(); // 서버에서 파괴, 클라이언트는 자동으로 리플리케이트되어 사라짐
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("InterestManager initialized with %d proxies from placed monsters."), MonsterProxies.Num());
    GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &AInterestManager::UpdateMonstersState, UpdateInterval, true);
}

void AInterestManager::UpdateMonstersState()
{
    if (!HasAuthority()) return; // ◀ 서버에서만 실행

    // 모든 플레이어의 위치를 수집합니다.
    TArray<FVector> AllPlayerLocations;
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PlayerController = It->Get();
        if (PlayerController && PlayerController->GetPawn())
        {
            AllPlayerLocations.Add(PlayerController->GetPawn()->GetActorLocation());
        }
    }

    // 플레이어가 한 명도 없으면 몬스터를 모두 Dormant 상태로 전환 (선택 사항)
    if (AllPlayerLocations.Num() == 0)
    {
        for (FMonsterProxyData& Proxy : MonsterProxies)
        {
            if (Proxy.CurrentState != EAIState::Dormant)
            {
                if (Proxy.SpawnedActorPtr.IsValid())
                {
                    Proxy.LastKnownLocation = Proxy.SpawnedActorPtr->GetActorLocation();
                    DespawnMonsterToPool(Proxy.SpawnedActorPtr.Get());
                    Proxy.SpawnedActorPtr = nullptr;
                }
                Proxy.CurrentState = EAIState::Dormant;
            }
        }
        return;
    }

    for (FMonsterProxyData& Proxy : MonsterProxies)
    {
        const FVector MonsterLocation = Proxy.SpawnedActorPtr.IsValid() ? Proxy.SpawnedActorPtr->GetActorLocation() : Proxy.LastKnownLocation;

        EAIState DesiredState = EAIState::Dormant; // 기본은 휴면

        // 모든 플레이어와의 거리를 체크하여 몬스터의 최종 상태를 결정
        for (const FVector& PlayerLocation : AllPlayerLocations)
        {
            const float HorizontalDistanceSq = FVector::DistSquared2D(PlayerLocation, MonsterLocation);
            const float VerticalDistance = FMath::Abs(PlayerLocation.Z - MonsterLocation.Z);

            if (VerticalDistance <= VerticalTolerance)
            {
                if (HorizontalDistanceSq < FMath::Square(ActiveRadius))
                {
                    DesiredState = EAIState::Active; // 하나라도 Active 안에 들어오면 Active
                    break; // 이미 Active이므로 더 이상 다른 플레이어와 비교할 필요 없음
                }
                else if (HorizontalDistanceSq < FMath::Square(RelevantRadius))
                {
                    // Active가 아닌 경우에만 Relevant로 설정 (이미 Active이면 유지)
                    if (DesiredState != EAIState::Active)
                    {
                        DesiredState = EAIState::Relevant; // 하나라도 Relevant 안에 들어오면 Relevant
                    }
                }
            }
        }

        // 상태 전환 로직
        if (Proxy.CurrentState != DesiredState)
        {
            if (Proxy.CurrentState == EAIState::Dormant && DesiredState != EAIState::Dormant)
            {
                // 휴면 -> 활성/관찰 (스폰)
                Proxy.SpawnedActorPtr = SpawnMonsterFromPool(Proxy, DesiredState);
                if (Proxy.SpawnedActorPtr.IsValid())
                {
                    // 스폰된 몬스터의 상태를 리플리케이트하도록 설정 (필요시)
                    // SpawnedMonster->SetReplicates(true); 
                    // SpawnedMonster->SetReplicateMovement(true);
                }
            }
            else if (Proxy.CurrentState != EAIState::Dormant && DesiredState == EAIState::Dormant)
            {
                // 활성/관찰 -> 휴면 (디스폰)
                if (Proxy.SpawnedActorPtr.IsValid())
                {
                    Proxy.LastKnownLocation = Proxy.SpawnedActorPtr->GetActorLocation();
                    DespawnMonsterToPool(Proxy.SpawnedActorPtr.Get());
                    Proxy.SpawnedActorPtr = nullptr;
                }
            }
            // Relevant <-> Active 전환은 몬스터 액터 자체의 상태 (틱 활성화/비활성화, AI 로직 변경 등)를 변경해야 합니다.
            // 여기서는 스폰/디스폰만 처리하므로 추가적인 로직이 필요할 수 있습니다.
            // 예: SpawnedActorPtr->SetActorTickEnabled(DesiredState == EAIState::Active);
            // SpawnedActorPtr->SetAILogicQuality(DesiredState);
            if (Proxy.SpawnedActorPtr.IsValid())
            {
                // 몬스터의 틱 활성화/비활성화
                Proxy.SpawnedActorPtr->SetActorTickEnabled(DesiredState == EAIState::Active || DesiredState == EAIState::Relevant);

                // 스폰된 몬스터에 DesiredState를 전달하여 내부적으로 로직 품질 변경
                // APPGASCharacterNonPlayer 클래스에 SetAIState 함수를 만들어 구현
                // Proxy.SpawnedActorPtr->SetAIState(DesiredState); 
            }
            Proxy.CurrentState = DesiredState;
        }
    }
}

APPGASCharacterNonPlayer* AInterestManager::SpawnMonsterFromPool(const FMonsterProxyData& ProxyData, EAIState DesiredState)
{
    if (!HasAuthority()) return nullptr; // 서버에서만 스폰

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSys == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("AInterestManager: Navigation System not found!"));
        return nullptr;
    }

    FNavLocation NavLocation;
    bool bFoundValidLocation = NavSys->GetRandomPointInNavigableRadius(ProxyData.LastKnownLocation, SpawnSearchRadius, NavLocation);

    if (bFoundValidLocation)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        // 서버에서 스폰된 액터는 자동으로 클라이언트로 리플리케이트됩니다.
        APPGASCharacterNonPlayer* SpawnedMonster = GetWorld()->SpawnActor<APPGASCharacterNonPlayer>(ProxyData.MonsterClass, NavLocation.Location, FRotator::ZeroRotator, SpawnParams);

        if (SpawnedMonster)
        {
            SpawnedMonster->InitialState = DesiredState; // 스폰된 몬스터에 초기 상태 전달
            // 스폰된 몬스터가 리플리케이션을 처리하도록 설정되어 있어야 합니다.
            // 몬스터 클래스 헤더에 SetReplicates(true) 가 기본적으로 설정되어 있는지 확인하세요.
            SpawnedMonster->SetReplicates(true);
            SpawnedMonster->SetReplicateMovement(true);
        }
        return SpawnedMonster;
    }

    UE_LOG(LogTemp, Warning, TEXT("Could not find valid spawn location on NavMesh near %s for monster."), *ProxyData.LastKnownLocation.ToString());
    return nullptr;
}

void AInterestManager::DespawnMonsterToPool(APPGASCharacterNonPlayer* MonsterToDespawn)
{
    if (!HasAuthority()) return; // 서버에서만 디스폰

    if (MonsterToDespawn)
    {
        // 서버에서 액터를 파괴하면 클라이언트에서는 자동으로 파괴가 리플리케이트됩니다.
        MonsterToDespawn->Destroy();
    }
}