// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EOSSubsystem.generated.h"


class UOnlineSubSystem;
class UEOSSubSystem;
class UGameDataClientSubsystem;

/**
 * 
 */
UCLASS()
class ONLINEFPS_API UEOSSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
#pragma region EOS Login
public:
	UFUNCTION(BlueprintCallable, Category="EOS Functions")
	void LoginWithEOS(FString ID, FString Token, FString LoginType);
	
	UFUNCTION(BlueprintCallable, Category="EOS Functions")
	void LogoutEOS();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="EOS Functions")
	FString GetPlayerUsername();
	UFUNCTION(BlueprintCallable, Category="EOS Functions")
	bool GetPlayerLoginStatus();
private:
	
	void LoginWithEOS_Callback(int32 LocalUserNum, bool bWasSucces, const FUniqueNetId& UserId, const FString& Error);
#pragma endregion
	
#pragma region EOS Session
	public:
		UFUNCTION(BlueprintCallable, Category="EOS Functions")
		void CreateSession(bool isDedicatedServer, bool isLanServer, int32 MaxPublicConnection);
		
		UFUNCTION(BlueprintCallable, Category="EOS Functions")
		void FindSession();
		
		UFUNCTION(BlueprintCallable, Category="EOS Functions")
		void JoinSession();
	
		//TSharedRef<FOnlineSessionSearch> sessionSearch;
	
	private:
		void OnCreateSessionComplete(FName SessionName, bool bWasSuccess);
		void OnFindSessionComplete(bool bWasSuccess);
		void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
#pragma endregion
};
