// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/EOShooterOnlineGameMode.h"
#include "TDMGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ONLINEFPS_API ATDMGameMode : public AEOShooterOnlineGameMode
{
	GENERATED_BODY()

private:
	int8 BlueTeamLives;
	
	int8 OrangeTeamLives;	

public:
	UPROPERTY(EditAnywhere)
	int8 MaxTeamAlive;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;
	virtual void OnPlayerKilled(AController* Victim, AController* Killer) override;
};
