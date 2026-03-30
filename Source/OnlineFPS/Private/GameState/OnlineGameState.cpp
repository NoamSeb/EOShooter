// Fill out your copyright notice in the Description page of Project Settings.

#include "GameState/OnlineGameState.h"
#include "Net/UnrealNetwork.h"

void AOnlineGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOnlineGameState, TeamScores);
}

void AOnlineGameState::OnRep_TeamScores()
{
	//TODO : RefreshHUD
	UE_LOG(LogTemp, Log, TEXT("Les scores ont été mis à jour sur le client !"));
}

void AOnlineGameState::AddScoreToTeam(ETeamRole Team, int32 Points)
{
	if (GetLocalRole() != ROLE_Authority) return;

	for (FTeamScoreData& Data : TeamScores)
	{
		if (Data.Team == Team)
		{
			Data.Score += Points;
			
			OnRep_TeamScores();
			break;
		}
	}
}