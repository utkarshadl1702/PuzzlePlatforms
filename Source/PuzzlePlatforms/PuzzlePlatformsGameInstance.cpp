// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzlePlatformsGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "PlatformTrigger.h"
#include "PuzzlePlatformsGameMode.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "OnlineSubsystem.h"

const static FName SESSION_NAME = NAME_GameSession;
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer &ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
    if (!ensure(MenuBPClass.Class != nullptr))
        return;

    // UE_LOG(LogTemp, Warning, TEXT("Found Class %s"),*   MenuBPClass.Class->GetName());
    MenuClass = MenuBPClass.Class;

    ConstructorHelpers::FClassFinder<UMenuWidget> InGameMenuBPClass(TEXT("/Game/WBP_InMenu"));
    if (!ensure(InGameMenuBPClass.Class != nullptr))
        return;

    // UE_LOG(LogTemp, Warning, TEXT("Found Class %s"),*   MenuBPClass.Class->GetName());
    InGameMenuClass = InGameMenuBPClass.Class;
}

void UPuzzlePlatformsGameInstance::Init()
{

    IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();

    if (Subsystem != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *Subsystem->GetSubsystemName().ToString());
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnCreateSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnDestroySessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnJoinSessionComplete);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Found no Subsystem"));
    }

    if(GEngine!=nullptr)
    {
        GEngine->OnNetworkFailure().AddUObject(this,&UPuzzlePlatformsGameInstance::OnNetworkFailure);
    }
}

void UPuzzlePlatformsGameInstance::LoadMenu()
{
    if (!ensure(MenuClass != nullptr))
        return;
    Menu = CreateWidget<UMainMenu>(this, MenuClass);
    if (!ensure(Menu != nullptr))
        return;
    Menu->Setup();
    Menu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::InGameLoadMenu()
{
    if (!ensure(InGameMenuClass != nullptr))
        return;
    UMenuWidget *_Menu = CreateWidget<UMenuWidget>(this, InGameMenuClass);
    if (!ensure(_Menu != nullptr))
        return;
    _Menu->Setup();
    _Menu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::Host(FString ServerName)
{
    DesiredServerName=ServerName;
    if (SessionInterface.IsValid())
    {
        auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
        if (ExistingSession != nullptr)
        {
            SessionInterface->DestroySession(SESSION_NAME);
        }
        else
        {
            CreateSession();
        }
    }
}

void UPuzzlePlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
    if (Success)
    {
        CreateSession();
    }
}
void UPuzzlePlatformsGameInstance::CreateSession()
{
    if (SessionInterface.IsValid())
    {
        FOnlineSessionSettings SessionSettings;
        if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
        {
            SessionSettings.bIsLANMatch = true;
        }
        else
        {
            SessionSettings.bIsLANMatch = false;
        }
        SessionSettings.NumPublicConnections = 3;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.bUseLobbiesIfAvailable = true;
        SessionSettings.Set(SERVER_NAME_SETTINGS_KEY,DesiredServerName,EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

        SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
    }

    // to add ping

    //     I wanted to add the ping as well, seemed pretty straightforward. Here's my solution.

    // Store ping as int32 as the ping from the result you will fetch is a int32.

    // USTRUCT()
    // struct FServerData
    // {
    //     GENERATED_BODY()
    //     FString HostUsername;
    //     FString Name;
    //     int32 Ping;
    //     uint16 CurrentPlayers;
    //     uint16 MaxPlayers;
    // };
    // Fetch the ping directly from the result.

    // FServerData ServerData;
    // ServerData.Ping = Result.PingInMs;
}

void UPuzzlePlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
    if (!Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
        return;
    }
    if (Menu != nullptr)
    {
        Menu->Teardown();
    }

    UEngine *Engine = GetEngine();

    if (!ensure(Engine != nullptr))
        return;

    Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, TEXT("Hosting"));
    UWorld *World = GetWorld();
    if (!ensure(World != nullptr))
        return;

    World->ServerTravel("/Game/PuzzlePlatforms/Maps/Lobby?listen");
}

void UPuzzlePlatformsGameInstance::RefreshServerList()
{
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    if (SessionSearch.IsValid())
    {
        // SessionSearch->bIsLanQuery = true;

        SessionSearch->MaxSearchResults = 100;

        SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

        UE_LOG(LogTemp, Warning, TEXT("Starting find Session"));
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }
}

void UPuzzlePlatformsGameInstance::StartSession()
{
    if(SessionInterface.IsValid())
    {
        SessionInterface->StartSession(SESSION_NAME);
    }
}

void UPuzzlePlatformsGameInstance::OnFindSessionsComplete(bool Success)
{
    if (Success && SessionSearch.IsValid() && Menu != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Finished find Sessions!!"));

        TArray<FServerData> ServerNames;

        for (const FOnlineSessionSearchResult &SearchResult : SessionSearch->SearchResults)
        {
            UE_LOG(LogTemp, Warning, TEXT("Found Session Names: %s"), *SearchResult.GetSessionIdStr());
            FServerData Data;
 
            Data.CurrentPlayers=Data.MaxPlayers-SearchResult.Session.NumOpenPublicConnections;
            Data.MaxPlayers=SearchResult.Session.SessionSettings.NumPublicConnections;
            Data.HostUserName=SearchResult.Session.OwningUserName;
            FString ServerName;
            if(SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY,ServerName))
            {
                Data.Name=ServerName;   
            }
            else
            {
                Data.Name="Couldnt find Name";               
            }
            ServerNames.Add(Data);
        }
        Menu->SetServerList(ServerNames);
    }
}

void UPuzzlePlatformsGameInstance::Join(uint32 Index)
{

    if (!SessionInterface.IsValid())
        return;
    if (!SessionSearch.IsValid())
        return;
    // if (Menu != nullptr)
    // {
    //     Menu->SetServerList({"Test1", "Test2"});
    // }

    SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UPuzzlePlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (!SessionInterface.IsValid())
        return;

    FString Address;
    if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not get connect string."));
        return;
    }

    UEngine *Engine = GetEngine();

    if (!ensure(Engine != nullptr))
        return;
    Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

    APlayerController *PlayerController = GetFirstLocalPlayerController();
    if (!ensure(PlayerController != nullptr))
        return;

    PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}


void UPuzzlePlatformsGameInstance::OnNetworkFailure(UWorld* World ,UNetDriver* NetDriver, ENetworkFailure::Type FailureType,const FString& ErrorString)
{
    LoadMainMenu();
}

void UPuzzlePlatformsGameInstance::LoadMainMenu()
{
    UWorld *World = GetWorld();
    if (World)
    {
        if (World->IsServer())
        {
            APuzzlePlatformsGameMode *GM = World->GetAuthGameMode<APuzzlePlatformsGameMode>();
            if (GM)
            {
                GM->ReturnToMainMenuHost();
            }
        }
        else
        {
            APlayerController *PC = GetFirstLocalPlayerController();
            if (PC)
            {
                PC->ClientReturnToMainMenu("Back to main menu");
            }
        }
    }
}