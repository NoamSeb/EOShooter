// Copyright Epic Games, Inc. All Rights Reserved.


#include "Public/Character/OnlineFPSPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "OnlineFPSCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "OnlineFPS.h"
#include "Widgets/Input/SVirtualJoystick.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemTypes.h"
#include "GameInstance/EOShooterGameInstance.h"
#include "Interfaces/OnlineIdentityInterface.h"


AOnlineFPSPlayerController::AOnlineFPSPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AOnlineFPSCameraManager::StaticClass();
}

void AOnlineFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogOnlineFPS, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void AOnlineFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
	
}

bool AOnlineFPSPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void AOnlineFPSPlayerController::OnNetCleanup(class UNetConnection* Connection)
{
	UEOShooterGameInstance* GI = Cast<UEOShooterGameInstance>(GetWorld()->GetGameInstance());
	if (GI)
	{
		GI->EOSSubsystem->DestroySession();
	}
	Super::OnNetCleanup(Connection);
}
