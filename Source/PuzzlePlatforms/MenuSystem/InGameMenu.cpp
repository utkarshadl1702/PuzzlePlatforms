// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMenu.h"
#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

bool UInGameMenu::Initialize()
{
    bool Success = Super::Initialize();
    if (!Success)
        return false;
    if (!ensure(CancelButton!= nullptr))
        return false;
    CancelButton->OnClicked.AddDynamic(this, &UInGameMenu::CancelPressed);


    if (!ensure(QuitButton!= nullptr))
        return false;
    QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::QuitPressed);




    return true;
}

void UInGameMenu::CancelPressed()
{
    Teardown();
    UGameplayStatics::SetGamePaused(GetWorld(),false);
}
void UInGameMenu::QuitPressed()
{
    if (MenuInterface!=nullptr)
    {
        Teardown();
        MenuInterface->LoadMainMenu();
    }
     
    
    
}


