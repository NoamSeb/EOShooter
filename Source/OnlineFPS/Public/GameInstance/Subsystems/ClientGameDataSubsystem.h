// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ClientGameDataSubsystem.generated.h"

UENUM(BlueprintType)
enum ClientGameState { 
	ECC_MAINMENU UMETA(DisplayName="Main Menu"),
	ECC_GAME UMETA(DisplayName="Game"),
};
/**
 * 
 */
UCLASS()
class ONLINEFPS_API UClientGameDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	ClientGameState CurrentGameState = ECC_MAINMENU;
	
	UFUNCTION(BlueprintCallable, Category="Game Data Functions")
	ClientGameState GetGameState();
	UFUNCTION(BlueprintCallable, Category="Game Data Functions")
	void SetGameState(ClientGameState NewState);
};
