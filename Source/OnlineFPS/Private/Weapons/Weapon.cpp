// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Weapon.h"

#include "Components/BoxComponent.h"


// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponCollideBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Collision"));
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	WeaponMesh->SetupAttachment(WeaponCollideBox);
	
	ShootingBulletPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Shooting Point"));
	ShootingBulletPoint->SetupAttachment(WeaponCollideBox);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

