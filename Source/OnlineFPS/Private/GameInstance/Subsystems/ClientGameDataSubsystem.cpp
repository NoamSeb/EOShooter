// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/Subsystems/ClientGameDataSubsystem.h"

ClientGameState UClientGameDataSubsystem::GetGameState()
{
	return CurrentGameState;
}

void UClientGameDataSubsystem::SetGameState(ClientGameState NewState)
{
	CurrentGameState = NewState;
}
