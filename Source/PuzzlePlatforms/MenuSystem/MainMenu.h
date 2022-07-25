// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

/**
 *
 */
USTRUCT()
struct FServerData
{
    GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUserName;
};
UCLASS()
class PUZZLEPLATFORMS_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()
public:
    UMainMenu(const FObjectInitializer &ObjectInitializer);	


	void SetServerList(TArray<FServerData> ServerNames);

	void SelectIndex(uint32 Index);



protected:
	virtual bool Initialize();

private:

	TSubclassOf<class UUserWidget> ServerRowClass;


	UPROPERTY(meta = (BindWidget))
	class UButton *Host;


	UPROPERTY(meta = (BindWidget))
	class UButton *HostDaGame;

	UPROPERTY(meta = (BindWidget))
	class UButton *Join;

	UPROPERTY(meta = (BindWidget))
	class UButton *QuitGame;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher *MenuSwitcher;


	UPROPERTY(meta = (BindWidget))
	class UButton *Cancel;

	UPROPERTY(meta = (BindWidget))
	class UButton *Fuckit;


	UPROPERTY(meta = (BindWidget))
	class UButton *Joindagame;


	UPROPERTY(meta = (BindWidget))
	class UWidget *JoinMenu;


	UPROPERTY(meta = (BindWidget))
	class UWidget *HostMenu;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox *ServerHostName;


	UPROPERTY(meta = (BindWidget))
	class UWidget *MainMenu;


	UPROPERTY(meta = (BindWidget))
	class UPanelWidget *ServerList;

	

	UFUNCTION()
	void HostServer();


	UFUNCTION()
	void HostSwitch();


	UFUNCTION()
	void Joingamefr();

	UFUNCTION()
	void OpenJoinMenu();


	UFUNCTION()
	void GoBack();

	UFUNCTION()
	void Quitdagame();	

	TOptional<uint32> SelectedIndex;

	void UpdateChildren();


};
