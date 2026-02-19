// Copyright Epic Games, Inc. All Rights Reserved.

#include "OnlineFPSGameMode.h"

#include "Character/OnlineFPSPlayerController.h"

AOnlineFPSGameMode::AOnlineFPSGameMode()
{
	// stub
	PlayerControllerClass = AOnlineFPSPlayerController::StaticClass();
}
