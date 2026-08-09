// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MetasoundSource.h"
#include "Weapon.generated.h"

class UBoxComponent;
UENUM(BlueprintType)
enum class EWeaponType: uint8
{
	ECC_Rifle UMETA(DisplayName = "Rifle"),
	ECC_Pistol UMETA(DisplayName = "Pistol")
};

UCLASS()
class ONLINEFPS_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> WeaponCollideBox = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> ShootingBulletPoint = nullptr;
	
	UPROPERTY(EditAnywhere, Category="FeedBack")
	TObjectPtr<UMetaSoundSource> ShootSounds = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FeedBack")
	TSubclassOf<UCameraShakeBase> CameraShakeBase = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FeedBack")
	float ShakeScale = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FeedBack")
	float RecoilScale = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Feedback")
	class UForceFeedbackEffect* ForceFeedbackAsset;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FeedBack")
	// float HapticForce = 1.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category = "Weapons Properties", meta = (ToolTip="Variable in meter", ForceUnits="m"))
	float WeaponRange;

	UPROPERTY(EditAnywhere, Category = "Weapons Properties", meta = (ToolTip="Variable in damage per shot", ForceUnits="d/s"))
	float WeaponDamage;

	UPROPERTY(EditAnywhere, Category = "Weapons Properties")
	EWeaponType WeaponType;
};
