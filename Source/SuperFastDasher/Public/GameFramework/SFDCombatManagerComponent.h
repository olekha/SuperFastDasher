// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../SuperFastDasher.h"
#include "SFDCombatManagerComponent.generated.h"

class UAnimMontage;
class ASFDCharacter;
class USFDComboMovesLibrary;
class UStaticMeshComponent;
struct FSFDMovesSequence;
struct FSFDMove;
class USFDPlayerInput;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SUPERFASTDASHER_API USFDCombatManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class USFDAnimNotifyState_MoveStage;

public:	
	// Sets default values for this component's properties
	USFDCombatManagerComponent();

	void SetActiveWeapon(UStaticMeshComponent* InNewActiveWeaponComponent);

	FORCEINLINE UStaticMeshComponent* GetActiveWeaponComponent() const
	{
		return ActiveUsedWaponComponent;
	}

	FORCEINLINE ESFDAttackState GetCurrentAttackState() const
	{
		return CurrentAttackState;
	}

	void StopCurrentSequence();

	const USFDComboMovesLibrary* GetComboMovesLibrary() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetComboState(const ESFDAttackState InAttackState);

private:

	void UpdateCombatState();

	const FSFDMovesSequence* const GetSequenceStartWithInput(const FName InInputName) const;

	const FSFDMove* GetNextMoveWithinSequence(const FSFDMovesSequence& InSequence, const int32 StartCountFrom) const;

	const FSFDMove* GetFirstMoveWithinSequence(const FSFDMovesSequence& InSequence) const;

	void PlayMoveInSequence(const FSFDMovesSequence* InNewSequence, const int32 InMoveIndex);

	bool TryToContinueCurrentSequence(/*const FSFDInputSnapshot& InWithInput, const bool bWasPopped*/);
	bool StartNewSequence(const FSFDInputSnapshot& InWithInput, const bool bWasPopped);

	bool IsPlayingMontage() const;

	ASFDCharacter* GetOwnerCharacter() const;

	UFUNCTION()
	void OnCurrentMotageEnded(UAnimMontage* Montage, bool bInterrupted);

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USFDComboMovesLibrary> ComboMovesLibrary;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ActiveUsedWaponComponent;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ESFDAttackState CurrentAttackState;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 CurrentMoveIndex = INDEX_NONE;

	FSFDMovesSequence* CurrentSequence = nullptr;

	UPROPERTY(Transient)
	USFDPlayerInput* pcInput;
};
