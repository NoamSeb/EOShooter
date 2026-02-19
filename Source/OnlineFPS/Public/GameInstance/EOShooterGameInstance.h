// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EOShooterGameInstance.generated.h"


class UOnlineSubSystem;
class UEOSSubSystem;
class UGameDataClientSubsystem;

/**
 * 
 */
UCLASS()
class ONLINEFPS_API UEOShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	
};
