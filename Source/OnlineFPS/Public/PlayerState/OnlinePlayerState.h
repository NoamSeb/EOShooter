// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OnlinePlayerState.generated.h"

enum class ETeamRole : uint8;

/**
 * 
 */
UCLASS()
class ONLINEFPS_API AOnlinePlayerState : public APlayerState
{
	GENERATED_BODY()
	
	ETeamRole CurrentTeam;

public:
	ETeamRole GetTeam();
};
