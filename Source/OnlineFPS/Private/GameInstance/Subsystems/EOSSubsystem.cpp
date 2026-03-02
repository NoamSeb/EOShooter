// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/Subsystems/EOSSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "EOSDeveloperSettings.h"
#include "Kismet/GameplayStatics.h"

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
			SessionSettings.bAllowJoinInProgress = true;
			SessionSettings.Set(TEXT("SEARCH_KEYWORDS"), FString("RandomHi"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
			SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSSubsystem::OnCreateSessionComplete);
			SessionPtr->CreateSession(0, FName("MainSession"), SessionSettings);
		}
	}
}

void UEOSSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccess)
{
	if (bWasSuccess)
	{
		IOnlineSubsystem* SubsystemPtr = Online::GetSubsystem(this->GetWorld());
		if (SubsystemPtr)
		{
			IOnlineSessionPtr SessionPtr = SubsystemPtr->GetSessionInterface();
			if (SessionPtr)
			{
				// 2. On récupère les données de la session qu'on vient de créer via son nom
				FNamedOnlineSession* NamedSession = SessionPtr->GetNamedSession(SessionName);

				if (NamedSession && NamedSession->SessionInfo.IsValid())
				{
					// 3. On extrait l'ID sous forme de String
					FString SessionID;
					SessionID = NamedSession->SessionInfo->GetSessionId().ToString();
					UE_LOG(LogTemp, Warning, TEXT("Session créée avec succès ! ID : %s"), *SessionID);
				}
			}
		}

		const UEOSDeveloperSettings* Settings = GetDefault<UEOSDeveloperSettings>();
		FString LevelToOpen = Settings->DefaultOpenLevelText;

		UE_LOG(LogTemp, Warning, TEXT("Voyage vers : %s"), *LevelToOpen);
		GetWorld()->ServerTravel(LevelToOpen + TEXT("?listen"));
	}
}


void UEOSSubsystem::DestroySession()
{
	IOnlineSubsystem* SubsystemPtr = Online::GetSubsystem(this->GetWorld());
	if (SubsystemPtr)
	{
		IOnlineSessionPtr SessionPtr = SubsystemPtr->GetSessionInterface();
		if (SessionPtr)
		{
			SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSSubsystem::OnDestroySessionComplete);
			SessionPtr->DestroySession(FName("MainSession"));
		}
	}
}

void UEOSSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccess)
{
}


void UEOSSubsystem::FindSession()
{
	IOnlineSubsystem* SubsystemPtr = Online::GetSubsystem(this->GetWorld());
	if (SubsystemPtr)
	{
		IOnlineSessionPtr SessionPtr = SubsystemPtr->GetSessionInterface();
		if (SessionPtr)
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->bIsLanQuery = true;
			SessionSearch->MaxSearchResults = 20;
			SessionPtr->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		}

//#if WITH_EDITOR
//		SessionSearch->QuerySettings.SearchParams.Empty();
//#else
		SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);
		SessionSearch->QuerySettings.Set(TEXT("SEARCH_KEYWORDS"), FString("RandomHi"), EOnlineComparisonOp::Equals);
//#endif
		FindSessionsCompleteDelegateHandle = SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(
			this, &UEOSSubsystem::OnFindSessionComplete);
		SessionPtr->FindSessions(0, SessionSearch.ToSharedRef());
	}
}


void UEOSSubsystem::OnFindSessionComplete(bool bWasSuccess)
{
	if (bWasSuccess)
	{
		IOnlineSubsystem* SubsystemPtr = Online::GetSubsystem(this->GetWorld());
		if (SubsystemPtr)
		{
			IOnlineSessionPtr SessionPtr = SubsystemPtr->GetSessionInterface();
			if (SessionPtr)
			{
				if (SessionSearch->SearchResults.Num() > 0)
				{
					SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSSubsystem::OnJoinSessionComplete);
					UE_LOG(LogTemp, Warning, TEXT("JOINING SESSION !"));
					SessionPtr->JoinSession(0, FName("MainSession"), SessionSearch->SearchResults[0]);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("NO SESSION FOUND !"));
					CreateSession(false, false, 10);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED FINDING SESSION !"));
		CreateSession(false, false, 10);
	}
}

void UEOSSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FString JoinAdress;
			IOnlineSubsystem* SubsystemPtr = Online::GetSubsystem(this->GetWorld());
			if (SubsystemPtr)
			{
				IOnlineSessionPtr SessionPtr = SubsystemPtr->GetSessionInterface();
				if (SessionPtr)
				{
					SessionPtr->GetResolvedConnectString(FName("MainSession"), JoinAdress);
					UE_LOG(LogTemp, Warning, TEXT("Join Adress : %s"), *JoinAdress);
					if (!JoinAdress.IsEmpty())
					{
						PC->ClientTravel(JoinAdress, TRAVEL_Absolute);
						UE_LOG(LogTemp, Warning, TEXT("SUCCESS JOINING SESSION !"));
					}
				}
			}
		}
	}
}

void UEOSSubsystem::JoinSession()
{
}
#pragma endregion
