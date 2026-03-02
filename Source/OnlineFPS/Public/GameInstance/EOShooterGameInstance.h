// Property of Noam Sebahoun

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Subsystems/EOSSubsystem.h"
#include "Subsystems/ClientGameDataSubsystem.h"
#include "EOShooterGameInstance.generated.h"

class UEOSSubsystem;
class UClientGameDataSubsystem;

/**
 * 
 */
UCLASS()
class ONLINEFPS_API UEOShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	virtual void Init() override;
	public:
	UEOSSubsystem* EOSSubsystem;
	UClientGameDataSubsystem* ClientGameDataSubsystem;
};
