// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DesertNinjasCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

//////////////////////////////////////////////////////////////////////////
// ADesertNinjasCharacter




ADesertNinjasCharacter::ADesertNinjasCharacter()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	

	// Create an orthographic camera (no perspective) and attach it to the boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 2048.0f;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->SetUsingAbsoluteRotation(true);
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

    // 	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
    // 	TextComponent->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
    // 	TextComponent->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
    // 	TextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    // 	TextComponent->SetupAttachment(RootComponent);

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;

	/** Init Params*/
	bIsAttacking = false;
	bIsThrowing = false;

	// By default the player is in this mode 
	bIdleWalkRun = true;

	// Stats params 
	MaxHealth = 100.f;
	BaseHealth = 65.f;
	MaxStamina = 150.f;
	BaseStamina = 120.f;
	Coins = 0;
}

//////////////////////////////////////////////////////////////////////////
// Animation

void ADesertNinjasCharacter::Attack()
{
	// Flag attack
	bIsAttacking = true;
	bIdleWalkRun = false;
}

void ADesertNinjasCharacter::ThrowObject()
{
	bIsThrowing = true;
	bIdleWalkRun = false;
}

void ADesertNinjasCharacter::UpdateAnimation()
{
	if (bIsAttacking) {
		// Attack only once
		bIsAttacking = false;

		UPaperFlipbook* DesiredAttackStyle = 
			(GetCharacterMovement()->IsFalling()) ? JumpAttackAnimation : AttackAnimation;
		if (GetSprite()->GetFlipbook() != DesiredAttackStyle)
		{
			GetSprite()->SetFlipbook(DesiredAttackStyle);
		}
		DelayIdleWalkRunAnimationUpdate(0.7f);
	}

	if (bIsThrowing) {
		bIsThrowing = false;
		DecreaseStamina();
		UPaperFlipbook* DesiredThrowingStyle =
			(GetCharacterMovement()->IsFalling()) ? ThrowObjectJumpAnimation : ThrowObjectAnimation;
		if (GetSprite()->GetFlipbook() != DesiredThrowingStyle)
		{
			GetSprite()->SetFlipbook(DesiredThrowingStyle);
		}
		DelayIdleWalkRunAnimationUpdate(0.7f);
	}

	if (bIdleWalkRun) {
		UpdateBasicAnimation();
	}
	
}

void ADesertNinjasCharacter::Jump() {
	Super::Jump();
	UE_LOG(LogTemp, Warning, TEXT("Jumping"));
	bIdleWalkRun = false;
	GetSprite()->SetFlipbook(JumpAnimation);
	DelayIdleWalkRunAnimationUpdate(0.7f);
}

void ADesertNinjasCharacter::UpdateBasicAnimation() {
	// Resume normal behavior 
	bIdleWalkRun = true;
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
	if( GetSprite()->GetFlipbook() != DesiredAnimation 	)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void ADesertNinjasCharacter::DecreaseStamina()
{
	if (BaseStamina > 10) {
		BaseStamina -= 10;
	}
}

void ADesertNinjasCharacter::IncrementCoins(int32 Amount)
{
	Coins += Amount;
}

void ADesertNinjasCharacter::IncrementHealth(float Amount)
{
	if (BaseHealth + Amount >= MaxHealth)
	{
		BaseHealth = MaxHealth;
	}
	else
	{
		BaseHealth += Amount;
	}
}

void ADesertNinjasCharacter::DecrementHealth(float Amount)
{
	if (BaseHealth - Amount <= 0.f)
	{
		BaseHealth -= Amount;
		UE_LOG(LogTemp, Warning, TEXT("DecrementHealth::Die()"));
		// Die();
	}
	else
	{
		BaseHealth -= Amount;
	}
}


void ADesertNinjasCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateCharacter();	
}


//////////////////////////////////////////////////////////////////////////
// Input

void ADesertNinjasCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual 
	// keys/buttons/sticks in DefaultInput.ini (editable from 
	// Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADesertNinjasCharacter::MoveRight);

	// Custom actions
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ADesertNinjasCharacter::Attack);
	PlayerInputComponent->BindAction("Throw", IE_Released, this, &ADesertNinjasCharacter::ThrowObject);
	

	PlayerInputComponent->BindTouch(IE_Pressed, this, &ADesertNinjasCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ADesertNinjasCharacter::TouchStopped);
}

void ADesertNinjasCharacter::MoveRight(float Value)
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void ADesertNinjasCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Jump on any touch
	Jump();
}

void ADesertNinjasCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Cease jumping once touch stopped
	StopJumping();
}

void ADesertNinjasCharacter::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();	
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

void ADesertNinjasCharacter::DelayIdleWalkRunAnimationUpdate(float duration) {
	GetWorldTimerManager().SetTimer(GeneralTimerHandler, this,
		&ADesertNinjasCharacter::UpdateBasicAnimation, duration, false);
}
