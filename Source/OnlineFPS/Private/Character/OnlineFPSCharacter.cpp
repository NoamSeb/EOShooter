// Copyright Epic Games, Inc. All Rights Reserved.

#include "Public/Character/OnlineFPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OnlineFPS.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/PaintDecal.h"

AOnlineFPSCharacter::AOnlineFPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	
	
	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f),
	                                                           FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh Component"));
}

void AOnlineFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOnlineFPSCharacter, CurrentLifeValue);
	DOREPLIFETIME(AOnlineFPSCharacter, PlayerMovementType);
	DOREPLIFETIME(AOnlineFPSCharacter, EquippedWeapon);
}

#pragma region Input
void AOnlineFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AOnlineFPSCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AOnlineFPSCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOnlineFPSCharacter::MoveInput);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AOnlineFPSCharacter::StopMove);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOnlineFPSCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this,
		                                   &AOnlineFPSCharacter::LookInput);

		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this,
		                                   &AOnlineFPSCharacter::CrouchInput);

		// Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this,
		                                   &AOnlineFPSCharacter::AttackInput);

		// Change Weapon
		EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Triggered, this,
		                                   &AOnlineFPSCharacter::DoChangeWeapon);
	}
	else
	{
		UE_LOG(LogOnlineFPS, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void AOnlineFPSCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AOnlineFPSCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);
}

void AOnlineFPSCharacter::CrouchInput(const FInputActionValue& Value)
{
	if (PlayerPosture != EPlayerPosture::ECC_Crouched)
	{
		if (PlayerMovementType != EPlayerState::ECC_Move)
			DoCrouch();
		else
			DoSlide();
	}
	else
	{
		PlayerPosture = EPlayerPosture::ECC_Standing;
		GEngine->AddOnScreenDebugMessage(true, 1.f, FColor::Red, TEXT("Standing"));
	}
}

void AOnlineFPSCharacter::AttackInput(const FInputActionValue& Value)
{
	if (PlayerMovementType != EPlayerState::ECC_Reloading)
		Attack();
}

void AOnlineFPSCharacter::ChangeWeaponInput(const FInputActionValue& Value)
{
	ChangeEquippedWeapon();
}
#pragma endregion

#pragma region InputReaction
void AOnlineFPSCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		PlayerMovementType = EPlayerState::ECC_Aim;
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AOnlineFPSCharacter::DoMove(float Right, float Forward)
{
	if (!GetController())
		return;

	if (!bCanMove)
		return;

	PlayerMovementType = EPlayerState::ECC_Move;

	// Apply movement
	AddMovementInput(GetActorRightVector(), Right);
	AddMovementInput(GetActorForwardVector(), Forward);

	// Dead zone to avoid noise from analog sticks
	const float DeadZone = 0.1f;

	const bool bForward = Forward > DeadZone;
	const bool bBackward = Forward < -DeadZone;
	const bool bRight = Right > DeadZone;
	const bool bLeft = Right < -DeadZone;

	// Determine direction
	if (bForward && bRight)
		MovementDirectionType = EMovementDirection::ECC_ForwardRight;
	else if (bForward && bLeft)
		MovementDirectionType = EMovementDirection::ECC_ForwardLeft;
	else if (bBackward && bRight)
		MovementDirectionType = EMovementDirection::ECC_BackwardRight;
	else if (bBackward && bLeft)
		MovementDirectionType = EMovementDirection::ECC_BackwardLeft;
	else if (bForward)
		MovementDirectionType = EMovementDirection::ECC_Forward;
	else if (bBackward)
		MovementDirectionType = EMovementDirection::ECC_Backward;
	else if (bRight)
		MovementDirectionType = EMovementDirection::ECC_Right;
	else if (bLeft)
		MovementDirectionType = EMovementDirection::ECC_Left;
}

void AOnlineFPSCharacter::StopMove()
{
	PlayerMovementType = EPlayerState::ECC_Idle;
}

void AOnlineFPSCharacter::DoJumpStart()
{
	// pass Jump to the character
	PlayerMovementType = EPlayerState::ECC_Jump;
	PlayerPosture = EPlayerPosture::ECC_Standing;
	Jump();
}

void AOnlineFPSCharacter::DoJumpEnd()
{
	PlayerMovementType = EPlayerState::ECC_Idle;
	// pass StopJumping to the character
	StopJumping();
}

void AOnlineFPSCharacter::DoCrouch()
{
	PlayerPosture = EPlayerPosture::ECC_Crouched;
	Crouch();
}

void AOnlineFPSCharacter::DoSlide()
{
	PlayerMovementType = EPlayerState::ECC_Slide;
	GEngine->AddOnScreenDebugMessage(true, 1.f, FColor::Green, TEXT("Sliding"));
	bCanMove = false;
	GetWorld()->GetTimerManager().SetTimer(
		SlideTimerHandle,
		this,
		&AOnlineFPSCharacter::EndSlide,
		1.0f,
		false
	);
}

void AOnlineFPSCharacter::EndSlide()
{
	bCanMove = true;
	PlayerPosture = EPlayerPosture::ECC_Crouched;
	PlayerMovementType = EPlayerState::ECC_Idle;
}

void AOnlineFPSCharacter::DoChangeWeapon()
{
	ChangeEquippedWeapon();
}

#pragma endregion InputReaction

void AOnlineFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	EquippedWeapon = PrimaryWeapon.GetDefaultObject();
	WeaponEquippedType = EquippedWeapon->WeaponType;
	WeaponMeshComponent->SetSkeletalMeshAsset(EquippedWeapon->WeaponMesh->GetSkeletalMeshAsset());
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponMeshComponent->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::KeepRelativeTransform, "HandGrip_R");
	WeaponMeshComponent->SetCanEverAffectNavigation(false);
	
	if (FirstPersonMesh && FirstPersonMesh->GetAnimInstance())
	{
		UpdateAnimLayer(WeaponEquippedType);
	}
}

void AOnlineFPSCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (AWeapon* TouchedWeapon = Cast<AWeapon>(OtherActor))
	{
		if (EquippedWeapon != nullptr)
			return;
		
		GrabWeapon(TouchedWeapon->WeaponMesh->GetSkeletalMeshAsset());
		EquippedWeapon = TouchedWeapon;
		OtherActor->Destroy();
	}
}

void AOnlineFPSCharacter::ChangeEquippedWeapon()
{
	if (EquippedWeapon == PrimaryWeapon.GetDefaultObject())
	{
		EquippedWeapon = SecondaryWeapon.GetDefaultObject();
		
	}else
	{
		EquippedWeapon = PrimaryWeapon.GetDefaultObject();
	}
	
	WeaponEquippedType = EquippedWeapon->WeaponType;
	WeaponMeshComponent->SetSkeletalMeshAsset(EquippedWeapon->WeaponMesh->GetSkeletalMeshAsset());
	WeaponMeshComponent->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::KeepRelativeTransform, "HandGrip_R");
	
	if (FirstPersonMesh && FirstPersonMesh->GetAnimInstance())
		UpdateAnimLayer(WeaponEquippedType);
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TON GRAND ONCLE CA MARCHE PAS FRR !"))
	}
}

void AOnlineFPSCharacter::UpdateAnimLayer(EWeaponType NewType)
{
	if (WeaponAnimLayers.Contains(NewType))
	{
		TSubclassOf<UAnimInstance> LayerClass = WeaponAnimLayers[NewType];

		if (FirstPersonMesh && LayerClass)
		{
			// 2. On récupère l'AnimInstance principal (Le Cerveau)
			UAnimInstance* MainAnimInst = FirstPersonMesh->GetAnimInstance();
            
			if (MainAnimInst)
			{
				// 3. LA MAGIE : On lie la nouvelle "cartouche" d'animations
				MainAnimInst->LinkAnimClassLayers(LayerClass);
			}else
			{
				UE_LOG(LogTemp, Warning, TEXT("NO ANIM INSTANCE FOUND !"));
			}
		}
	}
}

void AOnlineFPSCharacter::GrabWeapon(USkeletalMesh* WeaponToGrab)
{
	WeaponMeshComponent->SetSkeletalMeshAsset(WeaponToGrab);
	WeaponMeshComponent->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::KeepRelativeTransform, "HandGrip_R");
}

void AOnlineFPSCharacter::DropWeapon()
{
	WeaponMeshComponent->SetSkeletalMeshAsset(nullptr);
	AWeapon* DroppedWeapon = GetWorld()->SpawnActor<AWeapon>(EquippedWeapon.GetClass(), GetActorLocation(), GetActorRotation());
	EquippedWeapon = nullptr;
}

/// Client Side Attack Function
void AOnlineFPSCharacter::Attack()
{
	FVector Start = GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector ForwardCam = GetFirstPersonCameraComponent()->GetForwardVector();

	Server_Attack_Implementation(Start, ForwardCam);
}

/// Server Side Attack Function
/// @param Start Start point for the Line Trace
/// @param Forward Direction in which the Line Trace should Go
void AOnlineFPSCharacter::Server_Attack_Implementation(FVector_NetQuantize Start, FVector_NetQuantizeNormal Forward)
{
	float WeaponRange = EquippedWeapon? EquippedWeapon->WeaponRange : 10.f;
	
	//Multiply by 100.f because 100 Unreal units are equal to 1 meter
	FVector End = Start + (Forward * (WeaponRange * 100.f));

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("LineTrace")), true, this);

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
		if (AOnlineFPSCharacter* ActorToDamage = Cast<AOnlineFPSCharacter>(HitResult.GetActor()))
		{
			ActorToDamage->ReceiveDamage(EquippedWeapon->WeaponDamage);
			Client_ShowHitMarker(true);
		}
		else
		{
			Client_ShowHitMarker(false);
		}
				
		
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
		
		SpawnDecals_Implementation(HitResult.ImpactPoint, HitResult.ImpactNormal);
	}
}

/// Check if the Attack action is enable
/// @param Start 
/// @param Forward 
/// @return true if the player is can attack, false if the player can't attack
bool AOnlineFPSCharacter::Server_Attack_Validate(FVector_NetQuantize Start, FVector_NetQuantizeNormal Forward)
{
	float Tolerance = 100.0f;
	
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
	
	if (CurrentLifeValue <= 0 && PlayerMovementType == EPlayerState::ECC_Reloading)
	{
		return false;	
	}
	
	return true;
	
}

void AOnlineFPSCharacter::SpawnDecals_Implementation(FVector_NetQuantize SpawnLocation, FVector_NetQuantizeNormal ImpactNormal)
{
	FRotator TargetRotation = ImpactNormal.Rotation();
	AActor* SpawnedDecal = GetWorld()->SpawnActor<AActor>(Decal, SpawnLocation, TargetRotation);
}

void AOnlineFPSCharacter::Client_ShowHitMarker_Implementation(bool bShotPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("Player >Touched"));
}

void AOnlineFPSCharacter::ReceiveDamage(int ReceiveDamage)
{
	CurrentLifeValue -= ReceiveDamage;
	GEngine->AddOnScreenDebugMessage(true, 1.f, FColor::Green, FString::Printf(TEXT("Current life : %i"), CurrentLifeValue));
}
