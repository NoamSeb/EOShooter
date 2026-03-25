// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Weapons/PaintDecal.h"
#include "Weapons/Weapon.h"
#include "RangeEnnemyBot.generated.h"

UCLASS()
class ONLINEFPS_API ARangeEnnemyBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARangeEnnemyBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void AutomaticAttack();
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRate = 1.5f;

	FTimerHandle TimerHandle_Attack;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<APaintDecal> Decal = nullptr;
	
	UPROPERTY(Replicated)
	TObjectPtr<AWeapon> EquippedWeapon = nullptr;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Attack(FVector_NetQuantize Start, FVector_NetQuantizeNormal Forward);
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeapon;
	
	UFUNCTION(NetMulticast, Unreliable)
	void SpawnDecals(FVector_NetQuantize SpawnLocation, FVector_NetQuantizeNormal ImpactNormal);
	
private: 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
