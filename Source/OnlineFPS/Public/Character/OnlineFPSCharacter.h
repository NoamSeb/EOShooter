// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Weapons/Weapon.h"
#include "OnlineFPSCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class APaintDecal;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	ECC_Idle UMETA(DisplayName = "Idle"),
	ECC_Jump UMETA(DisplayName = "Jump"),
	ECC_Move UMETA(DisplayName = "Move"),
	ECC_Aim UMETA(DisplayName = "Aim"),
	ECC_Slide UMETA(DisplayName = "Slide"),
	ECC_Reloading UMETA(DisplayName = "Reloading"),
	ECC_ChangingWeapon UMETA(DisplayName = "Changing Weapon")
};
UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	ECC_Forward UMETA(DisplayName = "Forward"),
	ECC_Backward UMETA(DisplayName = "Backward"),
	ECC_Left UMETA(DisplayName = "Left"),
	ECC_Right UMETA(DisplayName = "Right"),
	ECC_ForwardLeft UMETA(DisplayName = "ForwardLeft"),
	ECC_ForwardRight UMETA(DisplayName = "ForwardRight"),
	ECC_BackwardLeft UMETA(DisplayName = "BackwardLeft"),
	ECC_BackwardRight UMETA(DisplayName = "BackwardRight")
};
UENUM(BlueprintType)
enum class EPlayerPosture : uint8
{
	ECC_Standing UMETA(DisplayName = "Standing"),
	ECC_Crouched UMETA(DisplayName = "Crouched"),
};

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AOnlineFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FullBodyMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:

	UPROPERTY()
	FVector2D MovementVector;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;
	
	/** Crouch / Slide Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* CrouchAction;
	
	/** Attack Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* AttackAction;
	
	/** Change Weapon Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* ChangeWeaponAction;
	
public:
	AOnlineFPSCharacter();
	
	UPROPERTY(EditAnywhere)
	int8 MaxLifeValue;
	
	UPROPERTY(EditAnywhere)
	int8 CurrentLifeValue;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> PrimaryWeapon;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> SecondaryWeapon;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APaintDecal> Decal = nullptr;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* WeaponMeshComponent;
	
protected:

	UFUNCTION(BlueprintCallable, Category = "Animations")
	void UpdateAnimLayer(EWeaponType NewType);
	
#pragma region Input
	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);
	
	/** Called from Input Actions for looking input */
	void CrouchInput(const FInputActionValue& Value);
	
	/** Called from Input Actions for looking input */
	void AttackInput(const FInputActionValue& Value);
	
	/** Called from Input Actions for looking input */
	void ChangeWeaponInput(const FInputActionValue& Value);

#pragma endregion

#pragma region InputReaction
	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void StopMove();

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	
	/** Handles Crouch Input */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoCrouch();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoSlide();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void EndSlide();
#pragma endregion InputReaction

	UFUNCTION()
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	UFUNCTION(BlueprintCallable, CallInEditor)
	virtual void ChangeEquippedWeapon();

	UFUNCTION(BlueprintCallable)
	void GrabWeapon(USkeletalMesh* WeaponToGrab);
	 
	UFUNCTION(BlueprintCallable, CallInEditor)
	void DropWeapon();


protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UPROPERTY(BlueprintReadOnly, Category="Movement")
	EPlayerState PlayerMovementType = EPlayerState::ECC_Idle;
	
	UPROPERTY(BlueprintReadOnly, Category="Movement")
	EPlayerPosture PlayerPosture = EPlayerPosture::ECC_Standing;
	
	UPROPERTY(BlueprintReadOnly, Category="Movement")
	EMovementDirection MovementDirectionType;

	UPROPERTY(BlueprintReadOnly, Category="Movement")
	EWeaponType WeaponEquippedType;

	UPROPERTY(EditAnywhere, Category = "Animations")
	TMap<EWeaponType, TSubclassOf<UAnimInstance>> WeaponAnimLayers;
	
	UFUNCTION(BlueprintCallable, Category="Action")
	void Attack();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Attack(FVector_NetQuantize Start, FVector_NetQuantizeNormal Forward);

	UFUNCTION(NetMulticast, Unreliable)
	void SpawnDecals(FVector_NetQuantize SpawnLocation, FVector_NetQuantizeNormal ImpactNormal);
	
	UFUNCTION(Client, Unreliable)
	void Client_ShowHitMarker(bool bShotPlayer);
	
	UFUNCTION(BlueprintCallable, Category="Action")
	void ReceiveDamage(int ReceiveDamage);
	
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	FTimerHandle SlideTimerHandle;

	UPROPERTY()
	TObjectPtr<AWeapon> EquippedWeapon = nullptr;
	
	UPROPERTY()
	bool bCanMove = true;
};

