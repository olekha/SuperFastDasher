// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SFDComboMovesLibrary.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FSFDMove
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FName> InputsNames;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* Animation;	

	UPROPERTY(EditDefaultsOnly)
	bool bUseWholeBody = true;
};

USTRUCT(BlueprintType)
struct FSFDMovesSequence
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FSFDMove> MovesList;
};

UCLASS(Blueprintable)
class SUPERFASTDASHER_API USFDComboMovesLibrary : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* DamageTakenAnimation;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* DamageTakenWithShieldAnimation;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* BlockAnimation;

	UPROPERTY(EditDefaultsOnly)
	TArray<FSFDMovesSequence> Sequences;
};
