// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SuperFastDasher.generated.h"

USTRUCT()
struct FSFDTimeSnapshot
{
	GENERATED_USTRUCT_BODY()

	template<typename T = FSFDTimeSnapshot>
	static T MakeSnapshot()
	{
		T Snapshot;

		const FDateTime NowTime = FDateTime::Now();

		Snapshot.Timestamp = static_cast<float>(NowTime.GetSecond()) + (static_cast<float>(NowTime.GetMillisecond()) * 0.001f);

		return Snapshot;
	}

	FORCEINLINE void Invalidate()
	{
		Timestamp = -1.0f;
	}

	FORCEINLINE bool IsValid() const
	{
		return Timestamp != -1.0f;
	}

	FORCEINLINE float GetTimestamp() const
	{
		return Timestamp;
	}

private:

	UPROPERTY(Transient)
	float Timestamp = -1.0f;
};

USTRUCT()
struct FSFDInputSnapshot : public FSFDTimeSnapshot
{
	GENERATED_USTRUCT_BODY()

	template<typename T = FSFDInputSnapshot>
	static T MakeSnapshot()
	{
		return FSFDTimeSnapshot::MakeSnapshot<FSFDInputSnapshot>();
	}

	FName InputName;

	bool bIsRepeted = false;
};

UENUM()
enum class ESFDAttackState : uint8
{
	None			UMETA(Hidden),
	Beginning,
	Damage,
	Finish,
	Max				UMETA(Hidden)
};

enum class ESFDInputPriority : uint8
{
	None			UMETA(Hidden),
	Permanent,
	Single,
	Max				UMETA(Hidden)
};

FString AttackStateToString(ESFDAttackState InAttackState);