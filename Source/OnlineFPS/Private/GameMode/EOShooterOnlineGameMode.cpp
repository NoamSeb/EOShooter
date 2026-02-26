// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/EOShooterOnlineGameMode.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

void AEOShooterOnlineGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NewPlayer)
	{
		FUniqueNetIdRepl UniqueNetIdRepl;
		if (NewPlayer->IsLocalController())
		{
			ULocalPlayer *LocalPlayerPtr = NewPlayer->GetLocalPlayer();
			if (LocalPlayerPtr)
			{
				UniqueNetIdRepl = LocalPlayerPtr->GetPreferredUniqueNetId();
			}else
			{
				UNetConnection *RemoteNetConectionPtr = Cast<UNetConnection>(NewPlayer->Player);
				check(IsValid(RemoteNetConectionPtr));
				UniqueNetIdRepl = RemoteNetConectionPtr->PlayerId;
			}
		}else
		{
			UNetConnection *RemoteNetConectionPtr = Cast<UNetConnection>(NewPlayer->Player);
			check(IsValid(RemoteNetConectionPtr));
			UniqueNetIdRepl = RemoteNetConectionPtr->PlayerId;
		}
		TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
		check(UniqueNetId != nullptr)
		IOnlineSubsystem* SubsystemPtr = Online::GetSubsystem(NewPlayer->GetWorld());
		IOnlineSessionPtr SessionPtr = SubsystemPtr->GetSessionInterface();
		bool bRegistrationSuccess = SessionPtr->RegisterPlayer(FName("MainSession"), *UniqueNetId, false);
		
		if(bRegistrationSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("Registering Successful for player %s"), *UniqueNetId->ToString());
		}
	}
	
}