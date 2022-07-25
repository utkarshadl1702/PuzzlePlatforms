// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "PuzzlePlatformsGameInstance.generated.h"

/**
 *
 */
UCLASS()
class PUZZLEPLATFORMS_API UPuzzlePlatformsGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()
public:
	UPuzzlePlatformsGameInstance(const FObjectInitializer &ObjectInitializer);
	virtual void Init();

	UFUNCTION(Exec, BlueprintCallable)
	void LoadMenu();

	UFUNCTION(Exec, BlueprintCallable)
	void InGameLoadMenu();

	UFUNCTION(Exec)
	void Host(FString ServerName) override;
	UFUNCTION(Exec)
	void Join(uint32 Index) override;

	virtual void LoadMainMenu() override;

	void RefreshServerList() override;

	void StartSession();

private:
	TSubclassOf<class UUserWidget> MenuClass;
	TSubclassOf<class UUserWidget> InGameMenuClass;

	class UMainMenu *Menu;
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	FString DesiredServerName;

	void OnNetworkFailure(UWorld* World ,UNetDriver* NetDriver, ENetworkFailure::Type FailureType,const FString& ErrorString);
	void CreateSession();
};
