// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "GameMapsSettings.h"
#include "UObject/SoftObjectPath.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "PuzzlePlatformsGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);
    ++NumberOfPlayers;
    
    if (NumberOfPlayers >= 3)
    {
        if (Countdown!=NULL)
        {
            Countdown->AddToViewport(1);
        }
        
        
        GetWorldTimerManager().SetTimer(GameStartTimer,this,&ALobbyGameMode::StartGame,10);
    }
}
void ALobbyGameMode::Logout(AController *Exiting)
{
    Super::Logout(Exiting);
    --NumberOfPlayers;
}

void ALobbyGameMode::StartGame()
{
    UPuzzlePlatformsGameInstance* GameInstance =Cast<UPuzzlePlatformsGameInstance>(GetGameInstance());
    if (GameInstance==nullptr)return;
    GameInstance->StartSession();
    UWorld *World = GetWorld();
    if (!ensure(World != nullptr))
        return;
    bUseSeamlessTravel = true;
    //   UGameMapsSettings::TransitionMap.SetPath("/Game/MenuSystem/LoadingScreen");
    World->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
}

