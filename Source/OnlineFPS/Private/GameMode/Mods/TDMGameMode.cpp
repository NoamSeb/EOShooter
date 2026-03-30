// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/Mods/TDMGameMode.h"

#include "PlayerState/OnlinePlayerState.h"

class AOnlinePlayerState;

void ATDMGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ATDMGameMode::BeginPlay()
{
	Super::BeginPlay();
	BlueTeamLives = MaxTeamAlive;
	OrangeTeamLives = MaxTeamAlive;
}

void ATDMGameMode::OnPlayerKilled(AController* Victim, AController* Killer)
{
	Super::OnPlayerKilled(Victim, Killer);

	if (Victim)
	{
		AOnlinePlayerState* VictimPS = Victim->GetPlayerState<AOnlinePlayerState>();
        
		if (VictimPS)
		{
			ETeamRole VictimTeam = VictimPS->GetTeam();
			
			UpdateTeamScore(VictimTeam, -1);
		}
	}
}
