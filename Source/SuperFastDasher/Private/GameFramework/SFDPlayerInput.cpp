// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/SFDPlayerInput.h"
#include "GameFramework/InputSettings.h"

const FName USFDPlayerInput::LightAttackInputName = "PrimaryAttack";
const FName USFDPlayerInput::HeavyAttackInputName = "SecondaryAttack";
const FName USFDPlayerInput::BlockInputName = "Block";
const FName USFDPlayerInput::MoveForwardInputName = "MoveForward";
const FName USFDPlayerInput::MoveRightInputName = "MoveRight";
const FName USFDPlayerInput::DashInputName = "Dash";

const TMap<FName, TArray<FName>> USFDPlayerInput::SpecialKeyNames = { {TEXT("Spec_AnyMove"), { MoveForwardInputName, MoveRightInputName }} };
const TArray<FName> USFDPlayerInput::BlockingKeyNames = { BlockInputName, DashInputName };

struct FSFDInputSnapshotSort
{
	FORCEINLINE bool operator()(const FSFDInputSnapshot& lhs, const FSFDInputSnapshot& rhs) const
	{
		return lhs.GetTimestamp() < rhs.GetTimestamp();
	}
};

struct FSFDOldInputSnapshot
{
	FORCEINLINE bool operator()(const FSFDInputSnapshot& lhs, const FSFDInputSnapshot& rhs) const
	{
		return lhs.GetTimestamp() < rhs.GetTimestamp();
	}
};

void USFDPlayerInput::ProcessInputStack(const TArray<UInputComponent*>& InputComponentStack, const float DeltaTime, const bool bGamePaused)
{
	Super::ProcessInputStack(InputComponentStack, DeltaTime,bGamePaused);

	for (int i = 0; i < InputSnapshots.Num(); ++i)
	{
		FSFDInputSnapshot& Snapshot = InputSnapshots[i];
		Snapshot.bIsRepeted = true;
	}

	const UInputSettings* InputSettings = GetDefault<UInputSettings>();

	const TArray <FInputActionKeyMapping>& CustomActionMuppings = InputSettings->GetActionMappings();
	const TArray <FInputAxisKeyMapping>& CustomAxisMuppings = InputSettings->GetAxisMappings();

	static TArray<TPair<FName, FKey>> JustProcessedKeys;
	JustProcessedKeys.Reserve(CustomActionMuppings.Num() + CustomAxisMuppings.Num());

	for (auto& ActionMupping : CustomActionMuppings)
	{
		if (WasJustPressed(ActionMupping.Key))
		{
			JustProcessedKeys.Add({ ActionMupping.ActionName, ActionMupping.Key });
		}
	}

	for (auto& AxisMupping : CustomAxisMuppings)
	{
		if (WasJustPressed(AxisMupping.Key))
		{
			JustProcessedKeys.Add({ AxisMupping.AxisName, AxisMupping.Key });
		}
	}

	for (const auto& Key : JustProcessedKeys)
	{
		FSFDInputSnapshot InputSnapshot = FSFDInputSnapshot::MakeSnapshot();
		InputSnapshot.InputName = Key.Key;

		InputSnapshots.Add(InputSnapshot);
	}

	if (!JustProcessedKeys.IsEmpty())
	{
		InputSnapshots.Sort(FSFDInputSnapshotSort());

		for (int i = InputSnapshots.Num() - 1; i >= 0 ; --i)
		{
			const FSFDInputSnapshot& Snapshot = InputSnapshots[i];
			if (BlockingKeyNames.Contains(Snapshot.InputName))
			{
				if (i > 0)
				{
					InputSnapshots.RemoveAt(0, i);
				}
				break;
			}
		}
	}

	JustProcessedKeys.Reset();

	CleanupOldInputs();
}

void USFDPlayerInput::CleanupOldInputs()
{
	const FSFDTimeSnapshot TimeSnapshot = FSFDTimeSnapshot::MakeSnapshot();

	if (InputSnapshots.RemoveAll([=](const FSFDInputSnapshot& Elem) { return FMath::Abs(TimeSnapshot.GetTimestamp() - Elem.GetTimestamp()) > InputLifeTimeInStack; }) > 0)
	{
		//
	}
}


