#include "InterestManager.h"
#include "Kismet/GameplayStatics.h"
#include "Character/PPGASCharacterNonPlayer.h"
#include "Monster/MS_Golem.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

AInterestManager::AInterestManager()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true; // 기본적으로 틱 활성화
    SetReplicatingMovement(false);
    bAlwaysRelevant = true;
    bReplicates = true;
}

void AInterestManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AInterestManager::BeginPlay()
{
    Super::BeginPlay();

    //UE_LOG(LogTemp, Warning, TEXT("AInterestManager::BeginPlay() called on %s"), HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"));

    // 서버에서만 매니저를 초기화하고 몬스터 상태 업데이트 타이머를 설정
    if (HasAuthority())
    {
        GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &AInterestManager::InitializeManager, 0.2f, false);
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
    if (!bShowDebugSpheres || GetWorld()->IsGameWorld() == false) return;

    if (HasAuthority())
    {
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* PlayerController = It->Get();
            if (PlayerController && PlayerController->GetPawn())
            {
                DrawDebugInterestZones(PlayerController->GetPawn()->GetActorLocation());
            }
        }
    }
    else
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
    if (!HasAuthority()) return;

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    /*if (!NavSys)
    {
        UE_LOG(LogTemp, Error, TEXT("InterestManager: Navigation System not found! Initialization failed."));
        return;
    }*/

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
                //UE_LOG(LogTemp, Warning, TEXT("Could not project monster spawn point to NavMesh for monster at %s. Using original location."), *NewProxy.LastKnownLocation.ToString());
            }

            NewProxy.CurrentState = EAIState::Dormant;
            MonsterProxies.Add(NewProxy);
            Monster->Destroy();
        }
    }

    //UE_LOG(LogTemp, Warning, TEXT("InterestManager initialized with %d proxies from placed monsters."), MonsterProxies.Num());
    GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &AInterestManager::UpdateMonstersState, UpdateInterval, true);
}

void AInterestManager::UpdateMonstersState()
{
    if (!HasAuthority()) return;

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

    // 플레이어가 한 명도 없으면 몬스터를 모두 Dormant 상태로 전환
    if (AllPlayerLocations.Num() == 0)
    {
        for (FMonsterProxyData& Proxy : MonsterProxies)
        {
            if (Proxy.bIsDead) continue;

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
        if (Proxy.bIsDead) continue;

        const FVector MonsterLocation = Proxy.SpawnedActorPtr.IsValid() ? Proxy.SpawnedActorPtr->GetActorLocation() : Proxy.LastKnownLocation;

        EAIState DesiredState = EAIState::Dormant;

        // 모든 플레이어와의 거리를 체크하여 몬스터의 최종 상태를 결정
        for (const FVector& PlayerLocation : AllPlayerLocations)
        {
            const float HorizontalDistanceSq = FVector::DistSquared2D(PlayerLocation, MonsterLocation);
            const float VerticalDistance = FMath::Abs(PlayerLocation.Z - MonsterLocation.Z);

            if (VerticalDistance <= VerticalTolerance)
            {
                if (HorizontalDistanceSq < FMath::Square(ActiveRadius))
                {
                    DesiredState = EAIState::Active;
                    break; // 이미 Active이므로 더 이상 다른 플레이어와 비교할 필요 없음
                }
                else if (HorizontalDistanceSq < FMath::Square(RelevantRadius))
                {
                    // Active가 아닌 경우에만 Relevant로 설정
                    if (DesiredState != EAIState::Active)
                    {
                        DesiredState = EAIState::Relevant;
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
            if (Proxy.SpawnedActorPtr.IsValid())
            {
                // 몬스터의 틱 활성화/비활성화
                Proxy.SpawnedActorPtr->SetActorTickEnabled(DesiredState == EAIState::Active || DesiredState == EAIState::Relevant);
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
        //UE_LOG(LogTemp, Error, TEXT("AInterestManager: Navigation System not found!"));
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
            SpawnedMonster->SetReplicates(true);
            SpawnedMonster->SetReplicateMovement(true);
        }
        return SpawnedMonster;
    }

    //UE_LOG(LogTemp, Warning, TEXT("Could not find valid spawn location on NavMesh near %s for monster."), *ProxyData.LastKnownLocation.ToString());
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

void AInterestManager::NotifyMonsterKilled(AActor* KilledMonster)
{
    if (!HasAuthority() || !KilledMonster) return;

    // 관리 중인 프록시 목록에서 해당 몬스터를 찾습니다.
    for (FMonsterProxyData& Proxy : MonsterProxies)
    {
        // 포인터가 일치하면 해당 프록시를 '사망' 처리합니다.
        if (Proxy.SpawnedActorPtr.Get() == KilledMonster)
        {
            Proxy.bIsDead = true;
            Proxy.SpawnedActorPtr = nullptr; // 죽었으므로 포인터 해제
            //UE_LOG(LogTemp, Log, TEXT("InterestManager: Monster Killed. Setting bIsDead to true."));
            break;
        }
    }
}