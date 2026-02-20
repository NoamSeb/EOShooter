// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance/EOShooterGameInstance.h"

void UEOShooterGameInstance::Init()
{
	Super::Init();
	EOSSubsystem = GetSubsystem<UEOSSubsystem>();
	ClientGameDataSubsystem = GetSubsystem<UClientGameDataSubsystem>();
	EOSSubsystem->LoginWithEOS("", "","PersistentAuth");
}