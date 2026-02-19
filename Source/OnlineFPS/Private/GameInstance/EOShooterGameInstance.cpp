// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance/EOShooterGameInstance.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

void UEOShooterGameInstance::Init()
{
	Super::Init();
	LoginWithEOS("", "","PersistentAuth");
}

void UEOShooterGameInstance::LoginWithEOS(FString ID, FString Token, FString LoginType)
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
			IdentityPointer->OnLoginCompleteDelegates->AddUObject(this, &UEOShooterGameInstance::LoginWithEOS_Callback);
			IdentityPointer->Login(0, AccountCredentials);
		}
	}
}

void UEOShooterGameInstance::LogoutEOS()
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

FString UEOShooterGameInstance::GetPlayerUsername()
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

bool UEOShooterGameInstance::GetPlayerLoginStatus()
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

void UEOShooterGameInstance::LoginWithEOS_Callback(int32 LocalUserNum, bool bWasSucces, const FUniqueNetId& UserId,
                                                   const FString& Error)
{
	
	FString SubsystemName = Online::GetSubsystem(GetWorld())->GetSubsystemName().ToString();
	UE_LOG(LogTemp, Warning, TEXT("Subsystem actuel utilisé : %s"), *SubsystemName);
	
	if (bWasSucces)
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Succes !"));
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("Login Fails ! Reason : %s"), *Error);
	}
}
