// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow2.h"
#include "Components/Button.h"
#include "MenuSystem/MainMenu.h"



void UServerRow2::Setup(class UMainMenu* InParent, uint32 InIndex)
{

    Parent=InParent;
    Index=InIndex;
    if (!ensure(RowButton != nullptr)) return;
    RowButton->OnClicked.AddDynamic(this, &UServerRow2::OnClicked);
}

void UServerRow2::OnClicked()
{
    Parent->SelectIndex(Index);
}
