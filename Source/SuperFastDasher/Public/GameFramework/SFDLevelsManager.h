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

#if WITH_EDITOR
	UFUNCTION(BlueprintCallable)
	void PrintAdjacencyMatrix_DEBUG();
	UFUNCTION(BlueprintCallable)
	void PrintIncidenceMatrix_DEBUG();
#endif
	
	static void GetAllConnectionsForRoom(TArray<uint8>& OutConnectedRoomsIndices, const uint8 InRoomIndex, const uint8*const *const InIncidenceMatrix, const uint8 InConnectionsAmount, const uint8 InRoomsAmount);
	static void InitAdjacency(uint8**& OutAdjacencyMatrixPtrPtr, const uint8 InDimension);
	// Find the minimum spanning tree and fill an Incidence Matrix
	static void FindMST(uint8**& OutIncidenceMatrixPtrPtr, const uint8*const *const InAdjacencyMatrixPtrPtr, const uint8 InAdjacencyMatrixDimension);
	static void ClearMatrix(uint8**& InAdjacencyMatrixPtrPtr, const uint8 InDimensionY);
	static void PrintMatrix(const uint8 * const * const InMatrix, const uint8 InSizeX, const uint8 InSizeY, const FString& InTitle);
	
	FORCEINLINE int8 GetPreviousRoomIndex() const;
	FORCEINLINE int8 GetCurrentRoomIndex() const;
	FORCEINLINE int8 GetPendingRoomIndex() const;

	FORCEINLINE uint8 GetRoomsAmount() const;
	FORCEINLINE uint8 GetConnectionsAmount() const;

	FORCEINLINE const uint8*const *const GetIncidenceMatrix() const;

	void SpawnStartRoom();
	void SpawnNextRoom(const ASFDNextRoomLoader* InFromRoomLoader);

	void GetAllConnectionsForRoom(TArray<uint8>& OutConnectedRoomsIndices, const uint8 InRoomIndex) const;
	
private:
		
	void SpawnRoom(const TSoftObjectPtr<UWorld>& InLevelClass, const FVector& InLevelLocation, const FRotator& InLevelRotation);

	void PrepareNextRoom();
	void ClearPreviousRoom();
	
	void ArrangeRooms();
	
	UFUNCTION()
	void OnNextLevelLoaded(/*const bool bIsFirstRoom*/);

	void TransportPlayerToNextLevel(const bool bSkipPreTeleportationDelay = false);
	UFUNCTION()
	void OnPlayerTransportationFinished();
	
private:
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess))
	TSoftObjectPtr<UWorld> RoomInstanceTemplate;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess))
	float TransitionBetweenRoomsCameraBlendTime = 3.0f;
	
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
	//@TODO temp, should be moved into custom ULevelStreamingDynamic
	bool bIsStartRoom = false;
};

FORCEINLINE int8 USFDLevelsManager::GetPreviousRoomIndex() const
{
	return PreviousRoomIndex;
}
	
FORCEINLINE int8 USFDLevelsManager::GetCurrentRoomIndex() const
{
	return CurrentRoomIndex;
}
	
FORCEINLINE int8 USFDLevelsManager::GetPendingRoomIndex() const
{
	return PendingRoomIndex;
}

FORCEINLINE uint8 USFDLevelsManager::GetRoomsAmount() const
{
	return RoomsAmount;
}

FORCEINLINE uint8 USFDLevelsManager::GetConnectionsAmount() const
{
	return ConnectionsAmount;
}

FORCEINLINE const uint8*const *const USFDLevelsManager::GetIncidenceMatrix() const
{
	return IncidenceMatrix;
}