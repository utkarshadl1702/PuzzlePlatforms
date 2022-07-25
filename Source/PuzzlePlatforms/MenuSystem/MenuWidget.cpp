// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

void  UMenuWidget::Setup()
{
    this->AddToViewport();
    UWorld* World=GetWorld();

    if (!ensure(World != nullptr)) return;


    APlayerController *PlayerController = World->GetFirstPlayerController();
    if (!ensure(PlayerController != nullptr)) return;
    FInputModeUIOnly InputData; 
    // InputData.SetUserFocus();   
    InputData.SetWidgetToFocus(this->TakeWidget());
    InputData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    PlayerController->SetInputMode(InputData); 

    PlayerController->bShowMouseCursor=true; 
}

void UMenuWidget::Teardown()
{

    this->RemoveFromViewport();
    UWorld* World=GetWorld();

    if (!ensure(World != nullptr)) return;
    APlayerController *PlayerController = World->GetFirstPlayerController();
    if (!ensure(PlayerController != nullptr)) return;
    FInputModeGameOnly InputData; 
    PlayerController->SetInputMode(InputData); 
    PlayerController->bShowMouseCursor=false; 

}

void  UMenuWidget::SetMenuInterface(IMenuInterface* _MenuInterface)
{
    this->MenuInterface=_MenuInterface;
}

void UMenuWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
     RemoveFromViewport();
     auto* world = GetWorld();
     if (!ensure(world != nullptr))
         return;
     auto* playerController = world->GetFirstPlayerController();
     if (!ensure(playerController != nullptr))
         return;
     FInputModeGameOnly inputMode;
     playerController->SetInputMode(inputMode);
     playerController->bShowMouseCursor = false;
     Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}