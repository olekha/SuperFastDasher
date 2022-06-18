// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "UObject/NoExportTypes.h"
#include "SFDLevelsManager.generated.h"

class ASFDNextRoomLoader;
class ULevelStreamingDynamic;
/**
 * 
 */
UCLASS(Blueprintable, HideDropdown)
class SUPERFASTDASHER_API USFDLevelsManager : public UObject
{
	GENERATED_BODY()

public:

	USFDLevelsManager();
	virtual ~USFDLevelsManager();
	
	UFUNCTION(BlueprintCallable)
	void PrintAdjacencyMatrix_DEBUG();
	
	UFUNCTION(BlueprintCallable)
	void PrintIncidenceMatrix_DEBUG();

	void PrintMatrix(const uint8 * const * const InMatrix, const uint8 InSizeX, const uint8 InSizeY, const FString& InTitle);
	
	void SpawnStartRoom();

	void SpawnNextRoom(const ASFDNextRoomLoader* InFromRoomLoader);

	void GetAllConnectionsForRoom(TArray<uint8>& OutConnectedRoomsIndices, const uint8 InRoomIndex) const;

	FORCEINLINE int8 GetPreviousRoomIndex() const
	{
		return PreviousRoomIndex;
	}
	
	FORCEINLINE int8 GetCurrentRoomIndex() const
	{
		return CurrentRoomIndex;
	}
	
	FORCEINLINE int8 GetPendingRoomIndex() const
	{
		return PendingRoomIndex;
	}
	
private:
		
	void SpawnRoom(const TSoftObjectPtr<UWorld>& InLevelClass, const FVector& InLevelLocation, const FRotator& InLevelRotation);

	void PrepareNextRoom();
	void ClearPreviousRoom();
	
	void InitAdjacency();

	// Find the minimum spanning tree
	void FindMST();

	UFUNCTION()
	void OnNextLevelLoaded();

	void TransportPlayerToNextLevel();

	void ClearAdjacencyMatrix();
	void ClearIncidenceMatrix();
	
private:
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess))
	TSoftObjectPtr<UWorld> RoomInstanceTemplate;
	
	UPROPERTY(Transient)
	ULevelStreamingDynamic* CurrentRoomDynamicInstance;
	
	UPROPERTY(Transient)
	ULevelStreamingDynamic* PendingRoomDynamicInstance;
		
	uint8** AdjacencyMatrix;
	uint8** IncidenceMatrix;

	uint8 RoomsAmount;
	uint8 ConnectionsAmount;
	
	//@TODO temp
	uint8 RoomIndexToStart = 0;

	int8 PreviousRoomIndex = INDEX_NONE;
	//@TODO temp, should be moved into custom ULevelStreamingDynamic
	int8 CurrentRoomIndex = INDEX_NONE;
	//@TODO temp, should be moved into custom ULevelStreamingDynamic
	int8 PendingRoomIndex = INDEX_NONE;
};
