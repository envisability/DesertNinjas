// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "DesertNinjasCharacter.generated.h"

class UTextRenderComponent;

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Running UMETA(DisplayName = "Running"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

/**
 * This class is the default character for DesertNinjas, and it is responsible for all
 * physical interaction between the player and the world.
 *
 * The capsule component (inherited from ACharacter) handles collision with the world
 * The CharacterMovementComponent (inherited from ACharacter) handles movement of the collision capsule
 * The Sprite component (inherited from APaperCharacter) handles the visuals
 */
UCLASS(config=Game)
class ADesertNinjasCharacter : public APaperCharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UTextRenderComponent* TextComponent;
	virtual void Tick(float DeltaSeconds) override;

	/** Movement component used for movement logic in various 
		movement modes (walking, falling, etc), containing relevant 
		settings and functions to control movement. */
	/*UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* MovementComponent;*/

protected:
	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	class UPaperFlipbook* RunningAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	// Attack animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* AttackAnimation;

	// Jump animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpAnimation;

	// Jump Attack animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpAttackAnimation;

	// Throw object animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* ThrowObjectAnimation;

	// Die animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* DieAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* StayDead;

	// Throw object while jumping animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* ThrowObjectJumpAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds")
	class USoundCue* WalkingSound;

	// Indicates the movement status of the player 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;


	/** Called for side to side input */
	void MoveRight(float Value);

	void UpdateCharacter();

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	/** Custom Animation Movements */
	// Attack using sword 
	void Attack();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Flags")
	bool bIsAttacking;

	// Action handler for throwing an object 
	void ThrowObject();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Flags")
	bool bIsThrowing;

	// The player is an idle, walk, or run state 
	bool bIdleWalkRun;
	virtual void Jump() override;
	 

public:
	ADesertNinjasCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

protected:
	// Delay
	FTimerHandle GeneralTimerHandler;
	void DelayIdleWalkRunAnimationUpdate(float duration);

	/** Called to choose the correct animation to play 
		based on the character's movement state */
	UFUNCTION()
	void UpdateAnimation();

	// Return to basic animation movement (idle to run)
	void UpdateBasicAnimation();

	// Sets the movement status of the character
	void SetMovementStatus(EMovementStatus Status);

	// Kill the character
	void Die();

	void SetStayDead();

public:
	/** Stats */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float BaseHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float BaseStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	/** Change player's vitals*/
	UFUNCTION(BlueprintCallable)
	void DecreaseStamina();

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	UFUNCTION(BlueprintCallable)
	void DecrementHealth(float Amount);

	/** Pickup */
	TArray<FVector> PickupLocations;

};
