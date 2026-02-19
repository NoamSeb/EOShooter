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
	
	virtual void Init() override;
	
	UFUNCTION(BlueprintCallable, Category="EOS Functions")
	void LoginWithEOS(FString ID, FString Token, FString LoginType);
	
	UFUNCTION()
	void LogoutEOS();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="EOS Functions")
	FString GetPlayerUsername();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="EOS Functions")
	bool GetPlayerLoginStatus();
	
	void LoginWithEOS_Callback(int32 LocalUserNum, bool bWasSucces, const FUniqueNetId& UserId, const FString& Error);
};
