// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/OnlinePlayerState.h"

ETeamRole AOnlinePlayerState::GetTeam()
{
	return CurrentTeam;
}

void AOnlinePlayerState::SetTeam(ETeamRole NewTeam)
{
	CurrentTeam = NewTeam;
}
