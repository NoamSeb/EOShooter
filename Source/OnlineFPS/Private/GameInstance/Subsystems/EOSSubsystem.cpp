// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/Subsystems/EOSSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSessionSettings.h"
#include "EOSDeveloperSettings.h"

#pragma region EOS Login
void UEOSSubsystem::LoginWithEOS(FString ID, FString Token, FString LoginType)
{
	IOnlineSubsystem* SubsystemPtr = Online::GetSubsystem(this->GetWorld());
	if (SubsystemPtr)
	{
		IOnlineIdentityPtr IdentityPointer = SubsystemPtr->GetIdentityInterface();
		if (IdentityPointer)
		{
			FOnlineAccountCredentials AccountCredentials;
			AccountCredentials.Id = ID;
			AccountCredentials.Token = Token;
			AccountCredentials.Type = LoginType;
			IdentityPointer->OnLoginCompleteDelegates->AddUObject(this, &UEOSSubsystem::LoginWithEOS_Callback);
			IdentityPointer->Login(0, AccountCredentials);
		}
	}
}


void UEOSSubsystem::LoginWithEOS_Callback(int32 LocalUserNum, bool bWasSucces, const FUniqueNetId& UserId,
                                          const FString& Error)
{
	if (bWasSucces)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Succes !"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed ! Reason : %s"), *Error);
	}
}

void UEOSSubsystem::LogoutEOS()
{
	IOnlineSubsystem* SubsystemPtr = Online::GetSubsystem(this->GetWorld());
	if (SubsystemPtr)
	{
		IOnlineIdentityPtr IdentityPointer = SubsystemPtr->GetIdentityInterface();
		if (IdentityPointer)
		{
			if (IdentityPointer->GetLoginStatus(0) == ELoginStatus::LoggedIn)
			{
				IdentityPointer->Logout(0);
			}
		}
	}
}

FString UEOSSubsystem::GetPlayerUsername()
{
	IOnlineSubsystem* SubsystemPtr = Online::GetSubsystem(this->GetWorld());
	if (SubsystemPtr)
	{
		IOnlineIdentityPtr IdentityPointer = SubsystemPtr->GetIdentityInterface();
		if (IdentityPointer)
		{
			if (IdentityPointer->GetLoginStatus(0) == ELoginStatus::LoggedIn)
			{
				return IdentityPointer->GetPlayerNickname(0);
			}
		}
	}
	return FString();
}

bool UEOSSubsystem::GetPlayerLoginStatus()
{
	IOnlineSubsystem* SubsystemPtr = Online::GetSubsystem(this->GetWorld());
	if (SubsystemPtr)
	{
		IOnlineIdentityPtr IdentityPointer = SubsystemPtr->GetIdentityInterface();
		if (IdentityPointer)
		{
			if (IdentityPointer->GetLoginStatus(0) == ELoginStatus::LoggedIn)
			{
				return true;
			}
		}
	}
	return false;
}


#pragma endregion

#pragma region EOS Session
void UEOSSubsystem::CreateSession(bool isDedicatedServer, bool isLanServer, int32 MaxPublicConnection)
{
	IOnlineSubsystem* SubsystemPtr = Online::GetSubsystem(this->GetWorld());
	if (SubsystemPtr)
	{
		IOnlineSessionPtr SessionPtr = SubsystemPtr->GetSessionInterface();
		if (SessionPtr)
		{
			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsDedicated = isDedicatedServer;
			SessionSettings.bAllowInvites = true;
			SessionSettings.bIsLANMatch = isLanServer;
			SessionSettings.NumPublicConnections = MaxPublicConnection;
			SessionSettings.bUseLobbiesIfAvailable = true;
			SessionSettings.bUsesPresence = true;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.Set("SEARCH_KEYWORDS", FString("RandomHi"), EOnlineDataAdvertisementType::ViaOnlineService);
			SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSSubsystem::OnCreateSessionComplete);
			SessionPtr->CreateSession(0, FName("MainSession"), SessionSettings);
		}
	}
}

void UEOSSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccess)
{
	if (bWasSuccess)
	{
		const UEOSDeveloperSettings* Settings = GetDefault<UEOSDeveloperSettings>();
		FString LevelToOpen = Settings->DefaultOpenLevelText;
        
		UE_LOG(LogTemp, Warning, TEXT("Voyage vers : %s"), *LevelToOpen);
		GetWorld()->ServerTravel(LevelToOpen + TEXT("?listen"));
	}
}

void UEOSSubsystem::FindSession()
{
}

void UEOSSubsystem::JoinSession()
{
}


#pragma endregion
