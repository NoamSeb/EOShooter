// Fill out your copyright notice in the Description page of Project Settings.


#include "Ennemy/RangeEnnemyBot.h"

#include "Character/OnlineFPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ARangeEnnemyBot::ARangeEnnemyBot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARangeEnnemyBot::BeginPlay()
{
	Super::BeginPlay();
	
	EquippedWeapon = DefaultWeapon.GetDefaultObject();

	// Si on est sur le serveur et que c'est une IA (ou un ennemi spécifique)
	if (HasAuthority()) 
	{
		GetWorldTimerManager().SetTimer(
			TimerHandle_Attack, 
			this, 
			&ARangeEnnemyBot::AutomaticAttack, 
			FireRate, 
			true // true = boucle infinie
		);
	}
}

// Called every frame
void ARangeEnnemyBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARangeEnnemyBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARangeEnnemyBot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARangeEnnemyBot, EquippedWeapon);
}

void ARangeEnnemyBot::AutomaticAttack()
{
	// On récupère la position et la direction (ici vers l'avant de l'acteur)
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();

	// Appelle ta fonction existante
	Server_Attack_Implementation(Location, Forward);
}

void ARangeEnnemyBot::Server_Attack_Implementation(FVector_NetQuantize Start, FVector_NetQuantizeNormal Forward)
{
	float WeaponRange = EquippedWeapon ? EquippedWeapon->WeaponRange : 10.f;
	
	//Multiply by 100.f because 100 Unreal units are equal to 1 meter
	FVector End = Start + (Forward * (WeaponRange * 100.f));

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("LineTrace")), false, this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		TraceParams
	);
	
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f);
	
	if (bHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();
		
		if (AOnlineFPSCharacter* ActorToDamage = Cast<AOnlineFPSCharacter>(HitResult.GetActor()))
		{
			UE_LOG(LogTemp, Warning, TEXT("DOMMAGES INFLIGÉS À : %s"), *ActorToDamage->GetName());
    
			// Debug visuel sur l'écran (apparaît en haut à gauche en jeu)
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("TOUCHÉ !"));
			}
			UGameplayStatics::ApplyDamage(ActorToDamage, 20.f, nullptr, this, UDamageType::StaticClass());
		}
		
		SpawnDecals_Implementation(HitResult.ImpactPoint, HitResult.ImpactNormal);
	}
}

/// Check if the Attack action is enable
/// @param Start 
/// @param Forward 
/// @return true if the player is can attack, false if the player can't attack
bool ARangeEnnemyBot::Server_Attack_Validate(FVector_NetQuantize Start, FVector_NetQuantizeNormal Forward)
{
	float Tolerance = 2000.0f;
	
	FVector ActorLocation = GetActorLocation();
	
	float Distance = FVector::Dist(Start, ActorLocation);
	
	if (Distance > Tolerance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Triche détectée : Distance de tir trop grande !"));
		return false;
	}
	
	if (!Forward.IsNormalized())
	{
		return false;
	}
	
	// if (CurrentLifeValue <= 0 && PlayerMovementType == EPlayerState::ECC_Reloading)
	// {
	// 	return false;	
	// }
	
	return true;
	
}

void ARangeEnnemyBot::SpawnDecals_Implementation(FVector_NetQuantize SpawnLocation, FVector_NetQuantizeNormal ImpactNormal)
{
	FRotator TargetRotation = ImpactNormal.Rotation();
	AActor* SpawnedDecal = GetWorld()->SpawnActor<AActor>(Decal, SpawnLocation, TargetRotation);
}