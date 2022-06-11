// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerInput.h"
#include "../SuperFastDasher.h"
#include "SFDPlayerInput.generated.h"

UCLASS(Blueprintable, hidedropdown)
class SUPERFASTDASHER_API USFDPlayerInput : public UPlayerInput
{
	GENERATED_BODY()

public:

	FORCEINLINE const TArray<FSFDInputSnapshot>& GetCurrentInputSnapshots() const
	{
		return InputSnapshots;
	}

	virtual void ProcessInputStack(const TArray<UInputComponent*>& InputComponentStack, const float DeltaTime, const bool bGamePaused) override;

private:

	void CleanupOldInputs();

public:

	static const FName LightAttackInputName;
	static const FName HeavyAttackInputName;
	static const FName BlockInputName;
	static const FName MoveForwardInputName;
	static const FName MoveRightInputName;
	static const FName DashInputName;

	static const TMap<FName, TArray<FName>> SpecialKeyNames;
	static const TArray<FName> BlockingKeyNames;

private:
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess))
	float InputLifeTimeInStack = 0.5f;

	TArray<FSFDInputSnapshot> InputSnapshots;
};
