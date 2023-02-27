// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SFDEnemyPawn.generated.h"

UENUM(BlueprintType)
enum class ESFDEnumAIState : uint8
{
	None			UMETA(Hidden),
	MeleeAttack,
	RangeAttack,
	GlidingAttack,
	IncreaseDistancing,
	CutDistance,
	MAX				UMETA(Hidden)	
};


class UInputComponent;

UCLASS()
class SUPERFASTDASHER_API ASFDEnemyPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASFDEnemyPawn();

	void SetState(const ESFDEnumAIState InNewState);

	FORCEINLINE ESFDEnumAIState GetState() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
private:

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess))
	ESFDEnumAIState CurrentState;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess))
	ESFDEnumAIState PreviousState;
	
	
};

FORCEINLINE ESFDEnumAIState ASFDEnemyPawn::GetState() const
{
	return CurrentState;
}