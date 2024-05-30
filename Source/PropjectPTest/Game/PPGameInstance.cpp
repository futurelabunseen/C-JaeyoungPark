
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
        SessionInterface = Subsystem->GetSessionInterface(); // ���� ��������
        if (SessionInterface.IsValid())
        {
            // ��������Ʈ�� ���� ����
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPPGameInstance::OnCreateSessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPPGameInstance::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPPGameInstance::OnJoinSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &ThisClass::OnDestroySessionComplete);
        }
    }
}

// ���� ����
void UPPGameInstance::CreateServer(const FCreateSessionInfo& InCreateSessionInfo)
{
    UE_LOG(LogPPNetwork, Warning, TEXT("Create Session"));

    if (!SessionInterface.IsValid()) return;

    if (SessionInterface->GetSessionState(NAME_GameSession) != EOnlineSessionState::NoSession) // ������ ���� ���ֱ�
    {
        bCreateSessionOnDestroy = true;
        DestroySession();
    }

    // ���� ����
    SessionSettings = MakeShareable(new FOnlineSessionSettings());
    if (!SessionSettings.IsValid())
    {
        UE_LOG(LogPPNetwork, Error, TEXT("SessionSettings is not valid"));
        return;
    }

    // ���� ����
    SessionSettings->bAllowJoinInProgress = true;		// �������� ���ǿ� ���� ��������?
    SessionSettings->bAllowJoinViaPresence = true;		// �÷��̾ ���� ���� ������ ��������? (ex. Ư�� �÷��̾ �������̸� �� �ش� ���ǿ� �����ϴ� ���)
    SessionSettings->bUseLobbiesIfAvailable = true;		// �÷����� �����ϴ� �κ� ����� ����� ���ΰ�?
    SessionSettings->bShouldAdvertise = true;		    // �¶��� ��ġ����ŷ�� �����Ǵ���?
    SessionSettings->bUsesPresence = true;		        // �����ڰ� ������ �¶��� ���¸� Ȯ���� �� �ִ���?
    SessionSettings->bIsLANMatch = InCreateSessionInfo.IsLan && IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";	// Lan ��ġ�ΰ�?
    SessionSettings->NumPublicConnections = InCreateSessionInfo.MaxPlayers;	//�ִ� ���� �ο�
    SessionSettings->BuildUniqueId = 1;		            // �˻� �߿� �ٸ� ���尡 ���� ������ �ʵ��� �ϴµ� ���ȴ�.

    // Ŭ���̾�Ʈ���� ������ �����͸� �����ϰ� ������ �� �ְ� �����Ѵ�.
    FString HostName = GetWorld() ? GetWorld()->GetFirstPlayerController()->PlayerState->GetPlayerName() : TEXT("Unknown");
    SessionSettings->Set(FName("SESSION_ROOM_NAME_KEY"), InCreateSessionInfo.RoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    SessionSettings->Set(FName("SESSION_HOSTNAME_KEY"), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    if (!SessionInterface->CreateSession(0, NAME_GameSession, *SessionSettings))
    {
        UE_LOG(LogPPNetwork, Warning, TEXT("Failed to Create Session"));
    }
}

// ������ ���������� �����Ѵٸ� �����󿡼� Ư�������� �̵�
void UPPGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
    UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete, Succeeded : %d"), Succeeded);
    if (Succeeded)
    {
        if (APlayerController* PC = GetFirstLocalPlayerController())
        { 
            //if (APPGASCharacter* Player = Cast<APPGASCharacter>(PC->GetPawn()))  // Ŭ���̾�Ʈ�� ���� ������ ������ �ִ��� Ȯ��
            //{
            /*if (PC->HasAuthority())
            {*/
                if (UWorld* World = GetWorld())
                {
                    // World->ServerTravel("/Game/Maps/ElvenRuins.ElvenRuins");
                    PC->ClientTravel("/Game/Maps/ElvenRuins.ElvenRuins", ETravelType::TRAVEL_Absolute); // URL�� ClientTravel ���� �̵�
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



// ���� Ž��
void UPPGameInstance::FindSession()
{
    UE_LOG(LogPPNetwork, Warning, TEXT("Find Session"));

    OnSearchingServer.Broadcast(true); // ���� �˻� �� (��ư ���й� �������� ��)
    SessionSearch = MakeShareable(new FOnlineSessionSearch);

    if (SessionSearch.IsValid())
    {
        if (IOnlineSubsystem::Get()->GetSubsystemName() != "NULL") // ���� ����ý����� NULL �̸� Lan ���� �˻� (������ ���������� NULL �� ��ȯ�ȴ�.)
        {
            SessionSearch->bIsLanQuery = false;
        }
        else
        {
            SessionSearch->bIsLanQuery = true; // Is Lan
        }
        SessionSearch->MaxSearchResults = 15000; // �˻� ��� �ִ밪
        SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals); // �˻� ���� �� ����
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }
    else
    {
        UE_LOG(LogPPNetwork, Error, TEXT("Failed to initialize SessionSearch"));
    }
}


// ������ ���������� Ž���ߴٸ� �ش� ���ǵ��� ������ ������ �̸� Broadcast ��
void UPPGameInstance::OnFindSessionsComplete(bool Succeeded)
{
    OnSearchingServer.Broadcast(false); // ���� �˻� �Ϸ�
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



// ���� ����
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

// Ÿ�� ���ǿ� ���������� �������� ��� ���� ������ Ŭ���̾�Ʈ �� �̵�
void UPPGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete, SessionName : %s"), *SessionName.ToString());
    if (APlayerController* PC = GetFirstLocalPlayerController())
    {
        FString JoinAddress = "";
        SessionInterface->GetResolvedConnectString(SessionName, JoinAddress); // ���� URL ��ȯ
        if (JoinAddress != "")
            PC->ClientTravel(JoinAddress, TRAVEL_Absolute); // URL�� ClientTravel ���� �̵�
    }
}

// ���� ����
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


// ����(ȣ��Ʈ)���� Ȯ���ϴ� �Լ� �߰�
//bool UPPGameInstance::IsServer() const
//{
//    if (GetWorld())
//    {
//        return GetWorld()->IsServer();
//    }
//    return false;
//}
