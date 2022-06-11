// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/SFDCombatManagerComponent.h"
#include "GameFramework/SFDComboMovesLibrary.h"
#include "Player/SFDCharacter.h"
#include "GameFramework/SFDPlayerInput.h"

// Sets default values for this component's properties
USFDCombatManagerComponent::USFDCombatManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void USFDCombatManagerComponent::SetActiveWeapon(UStaticMeshComponent* InNewActiveWeaponComponent)
{
	ActiveUsedWaponComponent = InNewActiveWeaponComponent;
}

// Called when the game starts
void USFDCombatManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UAnimInstance* AnimInstance = GetOwnerCharacter()->GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr)
	{
		AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &USFDCombatManagerComponent::OnCurrentMotageEnded);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetOwnerCharacter()->GetController()))
	{
		pcInput = Cast<USFDPlayerInput>(PC->PlayerInput);
	}
}

// Called every frame
void USFDCombatManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCombatState();

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, AttackStateToString(CurrentAttackState), true, FVector2D(1.5f, 1.5f));
}

void USFDCombatManagerComponent::UpdateCombatState()
{
	if (!ensure(ComboMovesLibrary != nullptr)
		|| !IsValid(pcInput))
	{
		SetComboState(ESFDAttackState::None);
		return;
	}

	if (CurrentAttackState == ESFDAttackState::Damage)
	{
		return;
	}

	const TArray<FSFDInputSnapshot>& CurrentInputsArray = pcInput->GetCurrentInputSnapshots();

	if (CurrentInputsArray.Num() <= 0)
	{
		return;
	}

	USFDComboMovesLibrary* MovesLibrary = ComboMovesLibrary->GetDefaultObject<USFDComboMovesLibrary>();

	const FSFDMovesSequence* NewSequenceToPlay = nullptr;
	int NewIndex = INDEX_NONE;
		
	if (CurrentSequence != nullptr)
	{
		if (CurrentSequence->MovesList.IsValidIndex(CurrentMoveIndex + 1)
			&& CurrentAttackState == ESFDAttackState::Finish)
		{
			const FSFDMove& Move = CurrentSequence->MovesList[CurrentMoveIndex + 1];

			bool bIsCorrectInputsChain = false;

			int l = 0;
			for (int k = 0; k < Move.InputsNames.Num(); ++k)
			{
				const FName& MoveInputName = Move.InputsNames[k];

				bIsCorrectInputsChain = false;
				for (l; l < CurrentInputsArray.Num(); l++)
				{
					const FSFDInputSnapshot& Snapshot = CurrentInputsArray[l];

					const TArray<FName>* const SpecKeyNames = USFDPlayerInput::SpecialKeyNames.Find(MoveInputName);

					if (MoveInputName == Snapshot.InputName || (SpecKeyNames != nullptr && SpecKeyNames->Contains(Snapshot.InputName)))
					{
						l++;
						bIsCorrectInputsChain = true;
						break;
					}
				}

				if (!bIsCorrectInputsChain)
				{
					break;
				}
			}

			if (bIsCorrectInputsChain)
			{
				NewSequenceToPlay = CurrentSequence;
				NewIndex = CurrentMoveIndex + 1;

				PlayMoveInSequence(NewSequenceToPlay, NewIndex);

				return;
			}
		}
	}

	NewSequenceToPlay = nullptr;
	NewIndex = INDEX_NONE;

	for (int i = 0; i < MovesLibrary->Sequences.Num(); ++i)
	{
		const FSFDMovesSequence& Sequence = MovesLibrary->Sequences[i];

		const FSFDMove& Move = Sequence.MovesList[0];

		bool bIsCorrectInputsChain = false;

		int l = 0;
		for (int k = 0; k < Move.InputsNames.Num(); ++k)
		{
			const FName& MoveInputName = Move.InputsNames[k];

			bIsCorrectInputsChain = false;
			for (l; l < CurrentInputsArray.Num(); l++)
			{
				const FSFDInputSnapshot& Snapshot = CurrentInputsArray[l];

				const TArray<FName>* const SpecKeyNames = USFDPlayerInput::SpecialKeyNames.Find(MoveInputName);

				if (MoveInputName == Snapshot.InputName || (SpecKeyNames != nullptr && SpecKeyNames->Contains(Snapshot.InputName)))
				{
					l++;
					bIsCorrectInputsChain = true;
					break;
				}
			}

			if (!bIsCorrectInputsChain)
			{
				break;
			}
		}

		const bool bHasMoreInputMatches = (NewSequenceToPlay == nullptr || Move.InputsNames.Num() > NewSequenceToPlay->MovesList[NewIndex].InputsNames.Num())
											&& (CurrentSequence == nullptr || Move.InputsNames.Num() > CurrentSequence->MovesList[CurrentMoveIndex].InputsNames.Num());

		if (bIsCorrectInputsChain && bHasMoreInputMatches)
		{
			NewSequenceToPlay = &Sequence;
			NewIndex = 0;
		}		
	}

	if (NewSequenceToPlay != nullptr)
	{
		PlayMoveInSequence(NewSequenceToPlay, NewIndex);
	}

	//if(CurrentSequence != nullptr)
	//{
	//	if (TryToContinueCurrentSequence(InputSnapshot, bWasPopped))
	//	{
	//		return;
	//	}
	//}

	//StartNewSequence(InputSnapshot, bWasPopped);
}

void USFDCombatManagerComponent::SetComboState(const ESFDAttackState InAttackState)
{
	if(CurrentAttackState != InAttackState)
	{
		CurrentAttackState = InAttackState;
	}
}

const FSFDMovesSequence* const USFDCombatManagerComponent::GetSequenceStartWithInput(const FName InInputName) const
{
	if (!ensure(ComboMovesLibrary != nullptr))
	{
		return nullptr;
	}

	USFDComboMovesLibrary* MovesLibrary = ComboMovesLibrary->GetDefaultObject<USFDComboMovesLibrary>();

	//for(const FSFDMovesSequence& Sequence : MovesLibrary->Sequences)
	//{
	//	if (!ensure(MovesLibrary->Sequences.IsEmpty()))
	//	{
	//		continue;
	//	}

	//	if(Sequence.MovesList[0].InputName.Num() == 1
	//		&& Sequence.MovesList[0].InputName[0].IsEqual(InInputName, ENameCase::CaseSensitive))
	//	{
	//		return &Sequence;
	//	}
	//}

	//for (const FSFDMovesSequence& Sequence : MovesLibrary->Sequences)
	//{
	//	if (!ensure(MovesLibrary->Sequences.IsEmpty()))
	//	{
	//		continue;
	//	}

	//	if (Sequence.MovesList[0].InputName.Num() == 2
	//		&& Sequence.MovesList[0].InputName[0].IsEqual(InInputName, ENameCase::CaseSensitive))
	//	{
	//		return &Sequence;
	//	}
	//}
	
	return nullptr;
}

const FSFDMove* USFDCombatManagerComponent::GetNextMoveWithinSequence(const FSFDMovesSequence& InSequence, const int32 StartCountFrom) const
{
	if(InSequence.MovesList.IsValidIndex(StartCountFrom))
	{
		return &InSequence.MovesList[StartCountFrom];
	}

	return nullptr;
}

const FSFDMove* USFDCombatManagerComponent::GetFirstMoveWithinSequence(const FSFDMovesSequence& InSequence) const
{
	return &InSequence.MovesList[0];
}

void USFDCombatManagerComponent::PlayMoveInSequence(const FSFDMovesSequence* InNewSequence, const int32 InMoveIndex)
{
	if (CurrentSequence != InNewSequence 
		|| CurrentMoveIndex != InMoveIndex)
	{
		if (InNewSequence != nullptr
			&& InNewSequence->MovesList.IsValidIndex(InMoveIndex))
		{
			CurrentMoveIndex = InMoveIndex;
			CurrentSequence = const_cast<FSFDMovesSequence*>(InNewSequence);

			GetOwnerCharacter()->PlayAnimMontage(CurrentSequence->MovesList[CurrentMoveIndex].Animation, CurrentSequence->MovesList[CurrentMoveIndex].bUseWholeBody);

			UE_LOG(LogTemp, Warning, TEXT("Anim Plyed %s"), *CurrentSequence->MovesList[CurrentMoveIndex].Animation->GetName());
		}
	}
}

bool USFDCombatManagerComponent::IsPlayingMontage() const
{
	return GetOwnerCharacter()->GetCurrentMontage() != nullptr;
}

void USFDCombatManagerComponent::StopCurrentSequence()
{
	if (CurrentSequence != nullptr)
	{
		GetOwnerCharacter()->StopAnimMontage(CurrentSequence->MovesList[CurrentMoveIndex].Animation);

		SetComboState(ESFDAttackState::None);
		CurrentMoveIndex = INDEX_NONE;
		CurrentSequence = nullptr;
	}
}

const USFDComboMovesLibrary* USFDCombatManagerComponent::GetComboMovesLibrary() const
{
	return ComboMovesLibrary != nullptr ? ComboMovesLibrary->GetDefaultObject<USFDComboMovesLibrary>() : nullptr;
}

bool USFDCombatManagerComponent::TryToContinueCurrentSequence(/*const FSFDInputSnapshot& InWithInput, const bool bWasPopped*/)
{
	if (CurrentSequence == nullptr
		|| CurrentMoveIndex == INDEX_NONE)
	{
		return false;
	}

	int32 NextMoveIndex = CurrentMoveIndex + 1;
	const FSFDMove* MoveToPerform = GetNextMoveWithinSequence(*CurrentSequence, NextMoveIndex);

	//if (MoveToPerform == nullptr
	//	|| !MoveToPerform->InputName.IsEqual(InWithInput.InputName, ENameCase::CaseSensitive))
	//{
	//	return false;
	//}

	PlayMoveInSequence(CurrentSequence, NextMoveIndex);

	return true;
}

bool USFDCombatManagerComponent::StartNewSequence(const FSFDInputSnapshot& InWithInput, const bool bWasPopped)
{
	if (CurrentSequence != nullptr)
	{
		StopCurrentSequence();
	}

	const FSFDMovesSequence* ProbableSequenceToStart = GetSequenceStartWithInput(InWithInput.InputName);

	if (ProbableSequenceToStart == nullptr)
	{
		return false;
	}

	PlayMoveInSequence(ProbableSequenceToStart, 0);

	return true;
}

ASFDCharacter* USFDCombatManagerComponent::GetOwnerCharacter() const
{
	ASFDCharacter* Character = Cast<ASFDCharacter>(GetOwner());

	ensure(Character != nullptr);

	return Character;
}

void USFDCombatManagerComponent::OnCurrentMotageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//UE_LOG(LogTemp, Warning, TEXT("MontageEnded Event from montage : %s"), *Montage->GetName());

	if (!bInterrupted)
	{
		StopCurrentSequence();
	}
}

