// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SFDMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SUPERFASTDASHER_API USFDMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	FORCEINLINE FVector GetLastNonZeroVelocity() const 
	{
		return LastNonZeroVelocity;
	}

protected:

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:

	FVector LastNonZeroVelocity;

};
