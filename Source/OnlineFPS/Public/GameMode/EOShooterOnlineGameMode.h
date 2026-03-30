// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EOShooterOnlineGameMode.generated.h"

UENUM(BlueprintType)
enum class ETeamRole : uint8
{
	None        UMETA(DisplayName = "Sans Équipe"),
	TeamAlpha   UMETA(DisplayName = "Alpha"),
	TeamBravo   UMETA(DisplayName = "Bravo"),
};

USTRUCT(BlueprintType)
struct FTeamInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere) FString TeamName;
	UPROPERTY(EditAnywhere) FColor TeamColor;
};

/**
 * 
 */
UCLASS()
class ONLINEFPS_API AEOShooterOnlineGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void OnPlayerKilled(AController* Victim, AController* Killer);
	
	void RequestRespawn(AController* Controller);

	UFUNCTION()
	void OnRep_TeamScores();
	
	void UpdateTeamScore(ETeamRole TeamToUpdate, int value);

	UPROPERTY(EditAnywhere, Category = "Teams")
	TMap<ETeamRole, FTeamInfo> ConfiguredTeams;

	UPROPERTY(BlueprintReadOnly)
	TMap<ETeamRole, int32> TeamScores;
};
