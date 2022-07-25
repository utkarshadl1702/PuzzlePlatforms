// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"
#include "Engine/Engine.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/EditableTextBox.h"
#include "PuzzlePlatforms/ServerRow2.h"
#include "Components/TextBlock.h"

UMainMenu::UMainMenu(const FObjectInitializer &ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
    if (!ensure(ServerRowBPClass.Class != nullptr))
        return;

    // UE_LOG(LogTemp, Warning, TEXT("Found Class %s"),*   MenuBPClass.Class->GetName());
    ServerRowClass = ServerRowBPClass.Class;
}

bool UMainMenu::Initialize()
{
    bool Success = Super::Initialize();
    if (!Success)
        return false;

    if (!ensure(Host != nullptr))
        return false;
    Host->OnClicked.AddDynamic(this, &UMainMenu::HostSwitch);


    if (!ensure(HostDaGame != nullptr))
        return false;
    HostDaGame->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

    if (!ensure(Join != nullptr))
        return false;
    Join->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

    if (!ensure(Cancel != nullptr))
        return false;
    Cancel->OnClicked.AddDynamic(this, &UMainMenu::GoBack);

    if (!ensure(Fuckit != nullptr))
        return false;
    Fuckit->OnClicked.AddDynamic(this, &UMainMenu::GoBack);

    if (!ensure(Joindagame != nullptr))
        return false;
    Joindagame->OnClicked.AddDynamic(this, &UMainMenu::Joingamefr);

    if (!ensure(QuitGame != nullptr))
        return false;
    QuitGame->OnClicked.AddDynamic(this, &UMainMenu::Quitdagame);

    return true;
}

void UMainMenu::HostServer()
{
    // UE_LOG(LogTemp, Warning, TEXT("I am gonna host a server bitch"));

    // if (MenuInterface != nullptr)
    // {
    //     MenuInterface->Host();
    // }
    if (MenuInterface != nullptr)
    {
        FString ServerName=ServerHostName->Text.ToString();
        MenuInterface->Host(ServerName);
    }


    // if (MenuInterface != nullptr)
    // {
    //     MenuInterface->RefreshServerList();
    // }
}

void UMainMenu::HostSwitch()
{
    if (!ensure(HostMenu != nullptr))
        return;
    if (!ensure(MenuSwitcher != nullptr))
        return;
    MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
    UWorld *World = this->GetWorld();
    if (!ensure(World != nullptr))
        return;

    ServerList->ClearChildren();

    uint32 i=0;
    for (const FServerData &ServerData : ServerNames)
    {
        UServerRow2 *Row = CreateWidget<UServerRow2>(World, ServerRowClass);
        if (!ensure(Row != nullptr))
            return;
        Row->ServerName->SetText(FText::FromString(ServerData.Name));
        Row->HostUser->SetText(FText::FromString(ServerData.HostUserName));

        FString FractionText=FString::Printf(TEXT("%d"),ServerData.MaxPlayers);
        Row->ConnectionFraction->SetText(FText::FromString(FractionText));
        Row->Setup(this,i);
        ++i;

        ServerList->AddChild(Row);
    }
}

void UMainMenu::SelectIndex(uint32 Index)
{
    SelectedIndex = Index;
    UpdateChildren();
}


void UMainMenu::UpdateChildren()
{
    for(int32 i=0; i < ServerList->GetChildrenCount();++i)
    {
        auto Row = Cast<UServerRow2>(ServerList->GetChildAt(i));
        if (Row!=nullptr)
        {
            Row->Selected=(SelectedIndex.IsSet() && SelectedIndex.GetValue()==i);
        }
        
    }
}

void UMainMenu::Joingamefr()
{
    if (SelectedIndex.IsSet() && MenuInterface != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected Index %d"), SelectedIndex.GetValue());
        MenuInterface->Join(SelectedIndex.GetValue());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected Index not Set"));
    }
}

void UMainMenu::OpenJoinMenu()
{
    if (!ensure(JoinMenu != nullptr))
        return;
    if (!ensure(MenuSwitcher != nullptr))
        return;
    MenuSwitcher->SetActiveWidget(JoinMenu);
    if (MenuInterface != nullptr)
    {
        MenuInterface->RefreshServerList();
    }
}
void UMainMenu::GoBack()
{
    if (!ensure(Cancel != nullptr))
        return;
    if (!ensure(MenuSwitcher != nullptr))
        return;
    MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::Quitdagame()
{
    UWorld *World = GetWorld();

    if (!ensure(World != nullptr))
        return;
    APlayerController *PlayerController = World->GetFirstPlayerController();

    if (!ensure(PlayerController != nullptr))
        return;
    PlayerController->ConsoleCommand("quit");
}


