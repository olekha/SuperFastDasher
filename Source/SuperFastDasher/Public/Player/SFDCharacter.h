// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../SuperFastDasher.h"
#include "SFDCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USFDMovementComponent;
class UCapsuleComponent;
class USFDVitalityComponent;
class USFDCombatManagerComponent;
struct FSFDMove;

UCLASS()
class SUPERFASTDASHER_API ASFDCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ASFDCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Sets default values for this character's properties
	
	bool IsAbleToBlockHit(const FVector& InHitDirection) const;
	FORCEINLINE bool IsBlockInitiated() const
	{
		return LastBlockSnapshot.IsValid();
	}
	
	bool IsAttacking() const;

	FORCEINLINE UStaticMeshComponent* GetWeaponMeshComponent() const 
	{ 
		return WeaponMesh;
	}

	FORCEINLINE USFDVitalityComponent* GetVitalityComponent() const
	{
		return SFDVitalityComponent;
	}

	void EndAttack();			

	void StartBlock();
	void EndBlock();

	bool IsTired() const;

	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;
	float PlayAnimMontage(class UAnimMontage* AnimMontage, bool OnWholeBody, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	void OnEnteredIntoRoomLoader();
	void OnStepOutFromRoomLoader();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector CalculateForwardMovementVector() const;
	FRotator CalculateRotationFromDirection() const;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:

	void MoveForward_Input(float AxisValue);
	void MoveRight_Input(float AxisValue);

	void Dash_Input();

	void PrimaryAttack_Input();	
	void SecondaryAttack_Input();

	void StartBlock_Input();
	void EndBlock_Input();

	void HandleMeshRotationTowardsDirection(const float DeltaSeconds);
	void HandleMovementSpeed();

private:

	UPROPERTY(Transient)
	FSFDTimeSnapshot LastBlockSnapshot;
	
	UPROPERTY(Transient)
	FVector BlockDirection = FVector::ZeroVector;
	
	UPROPERTY(Transient)
	FVector LastVelocityDirection = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShieldMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USFDVitalityComponent* SFDVitalityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USFDCombatManagerComponent* CombatManagerComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DashVelocity = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RotationInterpolationSpeed = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bDrawDebug = true;

	UPROPERTY(Transient)
	bool bIsAttacking_Primary = false;

	UPROPERTY(Transient)
	bool bIsAttacking_Secondary = false;
};
