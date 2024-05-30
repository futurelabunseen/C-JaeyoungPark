
#include "Game/PPGameInstance.h"
#include "Engine/World.h"
#include "PropjectPTest.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "Character/PPGASCharacter.h"

UPPGameInstance::UPPGameInstance()
{
}

void UPPGameInstance::Init()
{
    Super::Init();
    if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
    {
        SessionInterface = Subsystem->GetSessionInterface(); // 세션 가져오기
        if (SessionInterface.IsValid())
        {
            // 델리게이트로 세션 관리
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPPGameInstance::OnCreateSessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPPGameInstance::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPPGameInstance::OnJoinSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &ThisClass::OnDestroySessionComplete);
        }
    }
}

// 서버 생성
void UPPGameInstance::CreateServer(const FCreateSessionInfo& InCreateSessionInfo)
{
    UE_LOG(LogPPNetwork, Warning, TEXT("Create Session"));

    if (!SessionInterface.IsValid()) return;

    if (SessionInterface->GetSessionState(NAME_GameSession) != EOnlineSessionState::NoSession) // 동일한 세션 없애기
    {
        bCreateSessionOnDestroy = true;
        DestroySession();
    }

    // 세션 설정
    SessionSettings = MakeShareable(new FOnlineSessionSettings());
    if (!SessionSettings.IsValid())
    {
        UE_LOG(LogPPNetwork, Error, TEXT("SessionSettings is not valid"));
        return;
    }

    // 세션 설정
    SessionSettings->bAllowJoinInProgress = true;		// 진행중인 세션에 참가 가능한지?
    SessionSettings->bAllowJoinViaPresence = true;		// 플레이어를 통한 세션 참가가 가능한지? (ex. 특정 플레이어가 게임중이면 그 해당 세션에 참가하는 방법)
    SessionSettings->bUseLobbiesIfAvailable = true;		// 플랫폼이 제공하는 로비 기능을 사용할 것인가?
    SessionSettings->bShouldAdvertise = true;		    // 온라인 매치메이킹에 공개되는지?
    SessionSettings->bUsesPresence = true;		        // 참여자가 서로의 온라인 상태를 확인할 수 있는지?
    SessionSettings->bIsLANMatch = InCreateSessionInfo.IsLan && IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";	// Lan 매치인가?
    SessionSettings->NumPublicConnections = InCreateSessionInfo.MaxPlayers;	//최대 가용 인원
    SessionSettings->BuildUniqueId = 1;		            // 검색 중에 다른 빌드가 서로 보이지 않도록 하는데 사용된다.

    // 클라이언트에게 임의의 데이터를 생성하고 전달할 수 있게 설정한다.
    FString HostName = GetWorld() ? GetWorld()->GetFirstPlayerController()->PlayerState->GetPlayerName() : TEXT("Unknown");
    SessionSettings->Set(FName("SESSION_ROOM_NAME_KEY"), InCreateSessionInfo.RoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    SessionSettings->Set(FName("SESSION_HOSTNAME_KEY"), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    if (!SessionInterface->CreateSession(0, NAME_GameSession, *SessionSettings))
    {
        UE_LOG(LogPPNetwork, Warning, TEXT("Failed to Create Session"));
    }
}

// 세션을 성공적으로 생성한다면 서버상에서 특정맵으로 이동
void UPPGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
    UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete, Succeeded : %d"), Succeeded);
    if (Succeeded)
    {
        if (APlayerController* PC = GetFirstLocalPlayerController())
        { 
            //if (APPGASCharacter* Player = Cast<APPGASCharacter>(PC->GetPawn()))  // 클라이언트가 서버 권한을 가지고 있는지 확인
            //{
            /*if (PC->HasAuthority())
            {*/
                if (UWorld* World = GetWorld())
                {
                    // World->ServerTravel("/Game/Maps/ElvenRuins.ElvenRuins");
                    PC->ClientTravel("/Game/Maps/ElvenRuins.ElvenRuins", ETravelType::TRAVEL_Absolute); // URL로 ClientTravel 레벨 이동
                }
                else
                {
                    UE_LOG(LogPPNetwork, Error, TEXT("Failed to get World"));
                }
            //}+
            //}
        }
    }
}



// 세션 탐색
void UPPGameInstance::FindSession()
{
    UE_LOG(LogPPNetwork, Warning, TEXT("Find Session"));

    OnSearchingServer.Broadcast(true); // 세션 검색 중 (버튼 스패밍 막기위한 값)
    SessionSearch = MakeShareable(new FOnlineSessionSearch);

    if (SessionSearch.IsValid())
    {
        if (IOnlineSubsystem::Get()->GetSubsystemName() != "NULL") // 현재 서브시스템이 NULL 이면 Lan 으로 검색 (스팀이 꺼져있으면 NULL 로 반환된다.)
        {
            SessionSearch->bIsLanQuery = false;
        }
        else
        {
            SessionSearch->bIsLanQuery = true; // Is Lan
        }
        SessionSearch->MaxSearchResults = 15000; // 검색 결과 최대값
        SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals); // 검색 설정 값 지정
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }
    else
    {
        UE_LOG(LogPPNetwork, Error, TEXT("Failed to initialize SessionSearch"));
    }
}


// 세션을 성공적으로 탐색했다면 해당 세션들의 정보를 가져와 이를 Broadcast 함
void UPPGameInstance::OnFindSessionsComplete(bool Succeeded)
{
    OnSearchingServer.Broadcast(false); // 세션 검색 완료
    UE_LOG(LogPPNetwork, Warning, TEXT("Find Session %s"), Succeeded ? TEXT("Succeed") : TEXT("Failed"));

    if (Succeeded && SessionSearch.IsValid())
    {
        const TArray<FOnlineSessionSearchResult>& SearchResults = SessionSearch->SearchResults;
        if (SearchResults.Num() > 0)
        {
            for (const FOnlineSessionSearchResult& Result : SearchResults)
            {
                if (Result.IsValid())
                {
                    FServerInfo Info;

                    FString SessionName = "Empty Session Name";
                    FString HostName = "Empty Host Name";

                    Result.Session.SessionSettings.Get(FName("SESSION_ROOM_NAME_KEY"), SessionName);
                    Result.Session.SessionSettings.Get(FName("SESSION_HOSTNAME_KEY"), HostName);

                    Info.SessionName = SessionName;
                    Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
                    Info.CurrentPlayers = Info.MaxPlayers - Result.Session.NumOpenPublicConnections;

                    Info.SetPlayerCount();
                    Info.IsLan = Result.Session.SessionSettings.bIsLANMatch;
                    Info.Ping = Result.PingInMs;

                    UE_LOG(LogPPNetwork, Warning, TEXT("Found Session ID : %s"), *Result.GetSessionIdStr());

                    OnServerList.Broadcast(Info);
                }
            }
            UE_LOG(LogPPNetwork, Warning, TEXT("Session Count : %d"), SearchResults.Num());
        }
        else
        {
            UE_LOG(LogPPNetwork, Warning, TEXT("No valid session search results"));
        }
    }
}



// 세션 참가
void UPPGameInstance::JoinServer(int32 ArrayIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("Join Server"));
    FOnlineSessionSearchResult Result = SessionSearch->SearchResults[ArrayIndex];
    if (Result.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("JOINING AT INDEX : %d"), ArrayIndex);
        SessionInterface->JoinSession(0, CurrentSessionName, Result);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("FAILED TO JOIN SERVER AT INDEX : %d"), ArrayIndex);
    }
}

// 타겟 세션에 성공적으로 참여했을 경우 세션 맵으로 클라이언트 맵 이동
void UPPGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete, SessionName : %s"), *SessionName.ToString());
    if (APlayerController* PC = GetFirstLocalPlayerController())
    {
        FString JoinAddress = "";
        SessionInterface->GetResolvedConnectString(SessionName, JoinAddress); // 세션 URL 반환
        if (JoinAddress != "")
            PC->ClientTravel(JoinAddress, TRAVEL_Absolute); // URL로 ClientTravel 레벨 이동
    }
}

// 세션 삭제
void UPPGameInstance::DestroySession()
{
    if (SessionInterface->DestroySession(NAME_GameSession))
    {
        UE_LOG(LogPPNetwork, Warning, TEXT("Destroy Session Success"));
    }
    else
    {
        UE_LOG(LogPPNetwork, Warning, TEXT("Destroy Session Failed"));
    }
}

void UPPGameInstance::OnDestroySessionComplete(FName SessionName, bool Succeeded)
{
    UE_LOG(LogPPNetwork, Warning, TEXT("Session Name : %s /  %s"), *SessionName.ToString(), Succeeded ? TEXT("Succeed") : TEXT("Failed"));

    if (Succeeded)
    {
        if (!bCreateSessionOnDestroy)
        {
            if (UWorld* World = GetWorld())
            {
                if (APlayerController* PC = World->GetFirstPlayerController())
                {
                    PC->ClientTravel(MenuLevelPath, TRAVEL_Absolute);
                }
                else
                {
                    UE_LOG(LogPPNetwork, Error, TEXT("Failed to get PlayerController"));
                }
            }
            else
            {
                UE_LOG(LogPPNetwork, Error, TEXT("Failed to get World"));
            }
        }
        else
        {
            bCreateSessionOnDestroy = false;
        }
        UE_LOG(LogPPNetwork, Warning, TEXT("Session Destroyed"));
    }
    else
    {
        UE_LOG(LogPPNetwork, Warning, TEXT("Can't Destroy Session"));
    }
}


// 서버(호스트)인지 확인하는 함수 추가
//bool UPPGameInstance::IsServer() const
//{
//    if (GetWorld())
//    {
//        return GetWorld()->IsServer();
//    }
//    return false;
//}
