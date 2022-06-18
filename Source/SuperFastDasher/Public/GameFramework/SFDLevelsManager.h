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
	
private:
		
	void SpawnLevel(const TSoftObjectPtr<UWorld>& InLevelClass, const FVector& InLevelLocation, const FRotator& InLevelRotation);
	
	void InitAdjacency();

	// Find the minimum spanning tree
	void FindMST();

	UFUNCTION()
	void OnNextLevelLoaded();

	void TransportPlayer(const APlayerStart* InPlayerStart);
	
private:
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess))
	TSoftObjectPtr<UWorld> LevelInstance;
	
	UPROPERTY(Transient)
	ULevelStreamingDynamic* CurrentLevelDynamicInstance;
	
	UPROPERTY(Transient)
	ULevelStreamingDynamic* PendingLevelDynamicInstance;
		
	uint8** AdjacencyMatrix;
	uint8** IncidenceMatrix;

	static uint8 LevelsAmount;

	//@TODO temp
	uint8 LevelIndexToStart = 0;

	//@TODO temp, should be moved into custom ULevelStreamingDynamic
	int8 CurrentLevelIndex = INDEX_NONE;
	//@TODO temp, should be moved into custom ULevelStreamingDynamic
	int8 PendingLevelIndex = INDEX_NONE;
};
