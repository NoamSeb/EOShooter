// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameMode/EOShooterOnlineGameMode.h"
#include "OnlineGameState.generated.h"


USTRUCT(BlueprintType)
struct FTeamScoreData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ETeamRole Team = ETeamRole::None;

	UPROPERTY(BlueprintReadOnly)
	int32 Score = 0;
};

/**
 * 
 */
UCLASS()
class ONLINEFPS_API AOnlineGameState : public AGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(ReplicatedUsing = OnRep_TeamScores, BlueprintReadOnly, Category = "Teams")
	TArray<FTeamScoreData> TeamScores;
	
	UFUNCTION()
	void OnRep_TeamScores();
	
	void AddScoreToTeam(ETeamRole Team, int32 Points);
};
