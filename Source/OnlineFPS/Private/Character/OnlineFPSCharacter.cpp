// Copyright Epic Games, Inc. All Rights Reserved.

#include "Public/Character/OnlineFPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OnlineFPS.h"

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
}

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
	}
	else
	{
		UE_LOG(LogOnlineFPS, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

#pragma region Input
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
}
#pragma endregion

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

void AOnlineFPSCharacter::ChangeEquippedWeapon()
{
}

void AOnlineFPSCharacter::Attack()
{
	FVector Start = GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector ForwardCam = GetFirstPersonCameraComponent()->GetForwardVector();

	// TODO : Replace the coef by the weapon range
	FVector End = Start + (ForwardCam * 1000.0f);

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("LineTrace")), true, this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		TraceParams
	);
	
	if (bHit && HitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
	}

	// Optional: Draw the debug line for visualization
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f);
	
	if (bHit && HitResult.GetActor())
	{
		if (AOnlineFPSCharacter* ActorToDamage = Cast<AOnlineFPSCharacter>(HitResult.GetActor()))
			// TODO : Reaplce the Damage by the damage by weapon
			ActorToDamage->ReceiveDamage(10);
		// TODO : Add impact surface on wall or something
	}
	
}

void AOnlineFPSCharacter::ReceiveDamage(int ReceiveDamage)
{
	CurrentLifeValue -= ReceiveDamage;
	GEngine->AddOnScreenDebugMessage(true, 1.f, FColor::Green, FString::Printf(TEXT("Current life : %i"), CurrentLifeValue));
}
