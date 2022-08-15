// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFDLevelCore.generated.h"

class ATextRenderActor;
class ASFDNextRoomLoader;
class APlayerStart;
class ASFDLoadersSpawner;

UCLASS(HideDropdown)
class SUPERFASTDASHER_API ASFDLevelCore : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	ASFDLevelCore();

	ASFDNextRoomLoader* GetNextRoomLoaderByRoomIndex(const uint8 InRoomIndex) const;
	ASFDNextRoomLoader* GetNextRoomLoaderByLocalIndex(const uint8 InLocalIndex) const;
	
	FORCEINLINE int8 GetRoomIndex() const
	{
		return RoomIndex;
	}

	void InitializeRoom(const uint8 InRoomIndex);

	FORCEINLINE APlayerStart* GetPlayerStart() const
	{
		return PlayerStartActor;
	}

	FORCEINLINE void EnableTeleports();
	FORCEINLINE void DisableTeleports();
	FORCEINLINE bool IsTeleportsEnabled() const;
	FORCEINLINE float GetDistanceBetweenRooms() const;
	FORCEINLINE float GetNextRoomsZOffset() const;
	FORCEINLINE float GetNextRoomsZOffsetMinValue() const;
	FORCEINLINE FVector GetRoomCenterLocation() const;
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void ClearLoadersActors();
	
private:

	void UpdateNextRoomsLoaders();

	void OnPlayerEntersRoomLoader(uint8 InRoomIndex, uint8 InLoaderLocalIndex);
	void OnPlayerLeavesRoomLoader(uint8 InRoomIndex, uint8 InLoaderLocalIndex);
	
	void OnTransitionRequestReceived(const uint8 InTransitionRoomIndex);
	void OnTransitionRequestFulfilled(const uint8 InTransitionRoomIndex);
	
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
	ASFDLoadersSpawner* RoomLoadersSpawner;
	
	UPROPERTY(EditInstanceOnly)
	ASFDNextRoomLoader* PreviousRoomLoaderActor;

	UPROPERTY(EditInstanceOnly)
	float DistanceBetweenRooms = 2000.0f;

	UPROPERTY(EditInstanceOnly)
	float NextRoomsZOffset = 2000.0f;
	
	UPROPERTY(EditInstanceOnly)
	float NextRoomsZOffsetMinValue = 1000.0f;
	
	UPROPERTY(Transient)
	TArray<ASFDNextRoomLoader*> SpawnedLoaders;
		
	int8 RoomIndex = INDEX_NONE; 
	
	bool bIsTeleportsEnabled = true;
};

FORCEINLINE void ASFDLevelCore::EnableTeleports()
{
	bIsTeleportsEnabled = true;
}

FORCEINLINE void ASFDLevelCore::DisableTeleports()
{
	bIsTeleportsEnabled = false;
}

FORCEINLINE bool ASFDLevelCore::IsTeleportsEnabled() const
{
	return bIsTeleportsEnabled;
}

FORCEINLINE float ASFDLevelCore::GetDistanceBetweenRooms() const
{
	return DistanceBetweenRooms;
}

FORCEINLINE float ASFDLevelCore::GetNextRoomsZOffset() const
{
	return NextRoomsZOffset;
}

FORCEINLINE float ASFDLevelCore::GetNextRoomsZOffsetMinValue() const
{
	return NextRoomsZOffsetMinValue;
}

FORCEINLINE FVector ASFDLevelCore::GetRoomCenterLocation() const
{
	return GetActorLocation();
}