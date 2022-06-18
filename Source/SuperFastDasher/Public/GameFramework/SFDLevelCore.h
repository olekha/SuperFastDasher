// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SuperFastDasher/SuperFastDasher.h"
#include "SFDLevelCore.generated.h"

class ATextRenderActor;
class ASFDNextRoomLoader;
class APlayerStart;

UCLASS(HideDropdown)
class SUPERFASTDASHER_API ASFDLevelCore : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	ASFDLevelCore();

	ASFDNextRoomLoader* GetNextRoomLoaderByLocalIndex(const uint8 InLocalIndexIndex) const;
	
	const FTransform& GetNextRoomTransformByLocalIndex(const uint8 InLocalIndexIndex) const;

	FORCEINLINE int8 GetRoomIndex() const
	{
		return RoomIndex;
	}

	void SetRoomIndex(const uint8 InRoomIndex);

	FORCEINLINE APlayerStart* GetPlayerStart() const
	{
		return PlayerStartActor;
	}
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:

	void UpdateNextRoomsLoaders();

	uint8 GetFreeLocalIndexForNextRoom(const uint8 InRoomIndex) const;
	
private:
	
	UPROPERTY(EditInstanceOnly)
	APlayerStart* PlayerStartActor;

	UPROPERTY(EditInstanceOnly)
	ATextRenderActor* RoomIndexTextRenderActor;
	
	// [n] - (int)(n / 3)
	// [0] - 0..2
	// [1] - 3..5
	// [2] - 6..8
	// [3] - 9,10
	
	UPROPERTY(EditInstanceOnly)
	TArray<ASFDNextRoomLoader*> NextRoomLoaders;

	UPROPERTY(EditInstanceOnly)
	TArray<AActor*> RoomsSpawnPoints;

	int8 RoomIndex = INDEX_NONE; 
};
