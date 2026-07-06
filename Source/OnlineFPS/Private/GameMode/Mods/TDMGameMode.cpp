// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/Mods/TDMGameMode.h"

#include "PlayerState/OnlinePlayerState.h"

class AOnlinePlayerState;

void ATDMGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	NewPlayer->GetPlayerState<AOnlinePlayerState>()->SetTeam(ETeamRole::TeamAlpha);
}

void ATDMGameMode::BeginPlay()
{
	Super::BeginPlay();
	BlueTeamLives = MaxTeamAlive;
	OrangeTeamLives = MaxTeamAlive;
}

/*void ATDMGameMode::OnPlayerKilled(AController* Victim, AController* Killer)
{
	Super::OnPlayerKilled(Victim, Killer);

	if (Victim)
	{
		AOnlinePlayerState* VictimPS = Victim->GetPlayerState<AOnlinePlayerState>();
        
		if (VictimPS)
		{
			ETeamRole VictimTeam = VictimPS->GetTeam();
			
			UpdateTeamScore(VictimTeam, 1);
			RequestRespawn(VictimPS->GetPlayerController());
		}
	}
}*/

/*void ATDMGameMode::UpdateTeamScore(ETeamRole TeamToUpdate, int value)
{
	TeamScores[TeamToUpdate] -= value;
	
	if (TeamScores[TeamToUpdate] < 1)
	{
		FTeamInfo TeamDetails = ConfiguredTeams[TeamToUpdate];
		UE_LOG(LogTemp, Log, TEXT("GAME OVER ! %s LOSE !"), *TeamDetails.TeamName);
	}
	Super::UpdateTeamScore(TeamToUpdate, value);
}*/