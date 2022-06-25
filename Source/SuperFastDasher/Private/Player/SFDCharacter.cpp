// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SFDCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SFDVitalityComponent.h"
#include "GameFramework/SFDCombatManagerComponent.h"
#include "GameFramework/InputSettings.h"
#include <SuperFastDasher/Public/Player/Animations/SFDAnimInstance.h>
#include "GameFramework/SFDPlayerInput.h"
#include "GameFramework/SFDComboMovesLibrary.h"

// Sets default values
ASFDCharacter::ASFDCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if(GetCapsuleComponent() != nullptr)
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(51.0f);
		GetCapsuleComponent()->SetCapsuleRadius(26.0f);
		GetCapsuleComponent()->SetCollisionProfileName(FName("Pawn"));
	}

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 700.0f;
	SpringArmComponent->SocketOffset = FVector(0.0f, 0.0f, 430.0f);
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bUsePawnControlRotation = false;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetRelativeRotation(FRotator(-20.f, 0.0f, 0.0f));

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(GetMesh(), "Weapon_Socket");
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>("ShieldMesh");
	ShieldMesh->SetupAttachment(GetMesh(), "Shield_Socket");
	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SFDVitalityComponent = CreateDefaultSubobject<USFDVitalityComponent>("SFDVitalityComponent");

	CombatManagerComponent = CreateDefaultSubobject<USFDCombatManagerComponent>("CombatManagerComponent");
}

// Called when the game starts or when spawned
void ASFDCharacter::BeginPlay()
{
	Super::BeginPlay();	

	CombatManagerComponent->SetActiveWeapon(WeaponMesh);
}

// Called every frame
void ASFDCharacter::Tick(float DeltaTime)
{
	const FVector CurrCharVel = GetVelocity();
	if(!CurrCharVel.IsNearlyZero())
	{
		LastVelocityDirection = CurrCharVel.GetSafeNormal();
	}
	else if(IsBlockInitiated())
	{
		LastVelocityDirection = BlockDirection;
	}

	Super::Tick(DeltaTime);

	if(bDrawDebug)
	{
		const FVector CharVel = GetVelocity().GetSafeNormal();

		if(!CharVel.IsNearlyZero())
		{
			const FVector Start = GetActorLocation();
			const FVector End = Start + (CharVel * 100.0f);

			UKismetSystemLibrary::DrawDebugArrow(this, Start, End, 20.0f, FLinearColor::Green, 0.0f, 3.0f);
		}
		else
		{
			const FVector Start = GetActorLocation();
			const FVector End = Start + (LastVelocityDirection * 100.0f);

			UKismetSystemLibrary::DrawDebugArrow(this, Start, End, 20.0f, FLinearColor::Blue, 0.0f, 3.0f);
		}
	}
	
	HandleMeshRotationTowardsDirection(DeltaTime);
	
	HandleMovementSpeed();

	if (const APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (const USFDPlayerInput* pcInput = Cast<USFDPlayerInput>(PC->PlayerInput))
		{
			const TArray<FSFDInputSnapshot>& Array = pcInput->GetCurrentInputSnapshots();

			FString ComboString;

			for (int i = 0; i < Array.Num(); ++i)
			{
				ComboString += Array[i].InputName.ToString();

				if (i != Array.Num() - 1)
				{
					ComboString += " + ";
				}
			}

			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, ComboString, true, FVector2D(1.5f, 1.5f));

			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::FromInt(Array.Num()), true, FVector2D(1.0f, 1.0f));
		}
	}

	if (IsBlockInitiated())
	{
		const USFDAnimInstance* AnimInstance = Cast<USFDAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInstance != nullptr
				&& (AnimInstance->GetCurrentActiveMontage() == nullptr))
		{
			if (IsAttacking())
			{
				EndAttack();
			}
			
			if (CombatManagerComponent->GetComboMovesLibrary() != nullptr)
			{
				PlayAnimMontage(CombatManagerComponent->GetComboMovesLibrary()->BlockAnimation, false);
			}
		}
	}
}

// Called to bind functionality to input
void ASFDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(USFDPlayerInput::MoveForwardInputName, this, &ASFDCharacter::MoveForward_Input);
	PlayerInputComponent->BindAxis(USFDPlayerInput::MoveRightInputName, this, &ASFDCharacter::MoveRight_Input);
	
	PlayerInputComponent->BindAction(USFDPlayerInput::DashInputName, EInputEvent::IE_Pressed, this, &ASFDCharacter::Dash_Input);

	PlayerInputComponent->BindAction(USFDPlayerInput::LightAttackInputName, EInputEvent::IE_Pressed, this, &ASFDCharacter::PrimaryAttack_Input);
	PlayerInputComponent->BindAction(USFDPlayerInput::HeavyAttackInputName, EInputEvent::IE_Pressed, this, &ASFDCharacter::SecondaryAttack_Input);

	PlayerInputComponent->BindAction(USFDPlayerInput::BlockInputName, EInputEvent::IE_Pressed, this, &ASFDCharacter::StartBlock_Input);
	PlayerInputComponent->BindAction(USFDPlayerInput::BlockInputName, EInputEvent::IE_Released, this, &ASFDCharacter::EndBlock_Input);
}

float ASFDCharacter::PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	return PlayAnimMontage(AnimMontage, true, InPlayRate, StartSectionName);
}

float ASFDCharacter::PlayAnimMontage(UAnimMontage* AnimMontage, bool OnWholeBody, float InPlayRate, FName StartSectionName)
{
	USFDAnimInstance* AnimInstance = Cast<USFDAnimInstance>((GetMesh()) ? GetMesh()->GetAnimInstance() : nullptr);
	if (AnimInstance != nullptr)
	{
		AnimInstance->SetShouldUseWholeBody(OnWholeBody);
	}

	return Super::PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}

void ASFDCharacter::OnEnteredIntoRoomLoader()
{
	LastVelocityDirection = FVector::ZeroVector;

	if(GetCharacterMovement() != nullptr)
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
	}
}

void ASFDCharacter::OnStepOutFromRoomLoader()
{

}

float ASFDCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsAbleToBlockHit(FVector::ZeroVector))
	{
		FSFDTimeSnapshot CurrBlockSnapshot = FSFDTimeSnapshot::MakeSnapshot();

		if ((CurrBlockSnapshot.GetTimestamp() - LastBlockSnapshot.GetTimestamp()) <= 1.0f)
		{
			if (GEngine != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, FString("HIT PARRIED!!"));
			}
		}
		else
		{
			if (GEngine != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Blue, FString("HIT BLOCKED!!"));
			}

			if (CombatManagerComponent->GetComboMovesLibrary() != nullptr)
			{
				PlayAnimMontage(CombatManagerComponent->GetComboMovesLibrary()->DamageTakenWithShieldAnimation, false);
			}
		}

		return 0.0f;
	}

	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Blue, FString("DAMAGE TAKEN!!"));
	}

	if (SFDVitalityComponent != nullptr)
	{
		SFDVitalityComponent->AddHealth(Damage * -1.0f);
	}

	EndAttack();

	if (CombatManagerComponent->GetComboMovesLibrary() != nullptr)
	{
		PlayAnimMontage(CombatManagerComponent->GetComboMovesLibrary()->DamageTakenAnimation, false);
	}

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

FVector ASFDCharacter::CalculateForwardMovementVector() const
{
	return FVector(1.f, 0.0f, 0.0f);
}

FRotator ASFDCharacter::CalculateRotationFromDirection() const
{
	if(IsBlockInitiated())
	{
		return FRotator(0.0f, BlockDirection.Rotation().Yaw, 0.0f);
	}

	if(LastVelocityDirection.IsNearlyZero())
	{
		return GetActorRotation();
	}
	
	return FRotator(0.0f, LastVelocityDirection.Rotation().Yaw, 0.0f);
}

void ASFDCharacter::EndAttack()
{
	if (!IsAttacking())
	{
		return;
	}

	CombatManagerComponent->StopCurrentSequence();
}

void ASFDCharacter::StartBlock()
{
	if (IsBlockInitiated())
	{
		return;
	}

	LastBlockSnapshot = FSFDTimeSnapshot::MakeSnapshot();
	BlockDirection = GetActorRotation().Vector();
	
	if (IsAttacking())
	{
		EndAttack();
	}

	if (CombatManagerComponent->GetComboMovesLibrary() != nullptr)
	{
		PlayAnimMontage(CombatManagerComponent->GetComboMovesLibrary()->BlockAnimation, false);
	}
}

void ASFDCharacter::EndBlock()
{
	if (!IsBlockInitiated())
	{
		return;
	}

	LastBlockSnapshot.Invalidate();
	BlockDirection = FVector::ZeroVector;
	
	if (CombatManagerComponent->GetComboMovesLibrary() != nullptr)
	{
		StopAnimMontage(CombatManagerComponent->GetComboMovesLibrary()->BlockAnimation);
	}
}

bool ASFDCharacter::IsTired() const
{
	if (SFDVitalityComponent == nullptr)
	{
		return false;
	}

	return SFDVitalityComponent->GetCurrentStamina() <= 0;
}

bool ASFDCharacter::IsAbleToBlockHit(const FVector& InHitDirection) const
{
	bool bIsInBlockingAnim = false;

	USFDAnimInstance* AnimInstance = Cast<USFDAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance != nullptr)
	{
		bIsInBlockingAnim = AnimInstance->GetCurrentActiveMontage() == CombatManagerComponent->GetComboMovesLibrary()->BlockAnimation
			|| AnimInstance->GetCurrentActiveMontage() == CombatManagerComponent->GetComboMovesLibrary()->DamageTakenWithShieldAnimation;
	}

	return IsBlockInitiated() && bIsInBlockingAnim;
}

bool ASFDCharacter::IsAttacking() const
{
	return CombatManagerComponent->GetCurrentAttackState() != ESFDAttackState::None;
}

void ASFDCharacter::MoveForward_Input(float AxisValue)
{
	if (IsAttacking())
	{
		return;
	}

	const FVector NewForwardVector = CalculateForwardMovementVector();

	AddMovementInput(NewForwardVector, AxisValue);
}

void ASFDCharacter::MoveRight_Input(float AxisValue)
{
	if (IsAttacking())
	{
		return;
	}

	const FVector NewForwardVector = CalculateForwardMovementVector();

	const FVector NewRightVector = FVector::CrossProduct(NewForwardVector, FVector::ZAxisVector).GetSafeNormal();

	AddMovementInput(NewRightVector, AxisValue);
}

void ASFDCharacter::Dash_Input()
{
	const FVector VelocityDirection = GetVelocity().GetSafeNormal();
	const FVector CurrentLocation = GetActorLocation();

	const FVector FinalDashVelocity = CurrentLocation + (VelocityDirection * DashVelocity);

	FHitResult OutSweepHitResult;

	if (bDrawDebug)
	{
		const FVector Start = GetActorLocation();
		UKismetSystemLibrary::DrawDebugSphere(this, Start, 30.0f, 15.0f, FLinearColor::Red, 2.0f, 0.0f);

		const FVector End = FinalDashVelocity;
		UKismetSystemLibrary::DrawDebugSphere(this, End, 15.0f, 15.0f, FLinearColor::Red, 2.0f, 0.0f);
	}

	EndAttack();

	SetActorLocation(FinalDashVelocity, true , &OutSweepHitResult, ETeleportType::TeleportPhysics);
}

void ASFDCharacter::PrimaryAttack_Input()
{

}

void ASFDCharacter::SecondaryAttack_Input()
{

}

void ASFDCharacter::StartBlock_Input()
{
	StartBlock();
}

void ASFDCharacter::EndBlock_Input()
{
	EndBlock();
}

void ASFDCharacter::HandleMeshRotationTowardsDirection(const float DeltaSeconds)
{
	const FRotator CurrentBaseMeshRotation =  GetActorRotation();
	FRotator NewBaseMeshRotation = CalculateRotationFromDirection();

	FRotator Delta = CurrentBaseMeshRotation - NewBaseMeshRotation;
	Delta.Normalize();

	NewBaseMeshRotation = UKismetMathLibrary::RInterpTo(CurrentBaseMeshRotation, NewBaseMeshRotation, DeltaSeconds, RotationInterpolationSpeed);

	SetActorRotation(NewBaseMeshRotation, ETeleportType::TeleportPhysics);
}

void ASFDCharacter::HandleMovementSpeed()
{
	UCharacterMovementComponent* CharMoveComp = Cast<UCharacterMovementComponent>(GetMovementComponent());

	if(CharMoveComp == nullptr)
	{
		return;
	}

	CharMoveComp->MaxWalkSpeed = IsTired() ? 300.0f : 1000.0f;
}

void ASFDCharacter::StartPreTeleportationTimer(const FTransform& InTeleportationTransform)
{
	const UWorld* World = GetWorld();
	if(!ensureAlways(IsValid(World)))
	{
		return;
	}

	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "OnPreTeleportationTimerExpired", InTeleportationTransform);
	
	World->GetTimerManager().SetTimer(PrePostTeleportationTimerHandle, Delegate, PreTeleportationDelayTime, false);
}

void ASFDCharacter::OnPreTeleportationTimerExpired(const FTransform& InTeleportationTransform)
{
	TeleportPlayer(InTeleportationTransform);
}

void ASFDCharacter::StartPostTeleportationTimer()
{
	const UWorld* World = GetWorld();
	if(!ensureAlways(IsValid(World)))
	{
		return;
	}

	World->GetTimerManager().SetTimer(PrePostTeleportationTimerHandle, this, &ASFDCharacter::OnPostTeleportationTimerExpired, PostTeleportationDelayTime);
}

void ASFDCharacter::OnPostTeleportationTimerExpired()
{
	if(GetCharacterMovement() != nullptr)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

void ASFDCharacter::TeleportPlayer(const FTransform& InTeleportationTransform)
{
	const bool PrevCameraMovementLag = SpringArmComponent->bEnableCameraLag;
	const bool PrevCameraRotationLag = SpringArmComponent->bEnableCameraRotationLag;
	
	SpringArmComponent->bEnableCameraLag = false;
	SpringArmComponent->bEnableCameraRotationLag = false;

	SetActorLocation(InTeleportationTransform.GetLocation(), false, nullptr, ETeleportType::ResetPhysics);
	SetActorRotation(InTeleportationTransform.GetRotation(), ETeleportType::ResetPhysics);

	SpringArmComponent->bEnableCameraLag = PrevCameraMovementLag;
	SpringArmComponent->bEnableCameraRotationLag = PrevCameraRotationLag;
	
	if(OnPlayerTeleported.IsBound())
	{
		OnPlayerTeleported.Broadcast();
	}
	
	StartPostTeleportationTimer();
}

FTransform ASFDCharacter::GetCameraTransform() const
{
	return CameraComponent->GetComponentTransform();
}
