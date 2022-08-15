// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SFDLevelsManager.generated.h"

struct FSFDRoomTransitionData
{
	uint8 FromRoom = 0xFFu;
	uint8 FromRoomLoader = 0xFFu;
	uint8 ToRoom = 0xFFu;
	uint8 ToRoomLoader = 0xFFu;

	bool IsValid() const
	{
		return ToRoom != 0xFFu;
	}
	
	void Reset()
	{
		FromRoom = 0xFFu;
		FromRoomLoader = 0xFFu;
		ToRoom = 0xFFu;
		ToRoomLoader = 0xFFu;
	}
};

struct FSFDRoomNode
{
	uint8 RoomIndex;
	const FSFDRoomNode* PreviousRoom;
	TArray<const FSFDRoomNode*> NextRooms;

	FSFDRoomNode() : RoomIndex(0xFF), PreviousRoom(nullptr), NextRooms({})
	{
	}
	
	FSFDRoomNode(const uint8 InRoomIndex,  const FSFDRoomNode* InPreviousRoom, const TArray<const FSFDRoomNode*>& InNextRooms)
		: RoomIndex(InRoomIndex), PreviousRoom(InPreviousRoom), NextRooms(InNextRooms)
	{
	}

	~FSFDRoomNode()
	{
		PreviousRoom = nullptr;
		RoomIndex = 0xFF;

		if(!NextRooms.IsEmpty())
		{
			for(int8 i = NextRooms.Num() - 1; i >= 0; --i)
			{
				if(NextRooms.IsValidIndex(i) && NextRooms[i] != nullptr)
				{
					delete NextRooms[i];
				}
			}
		}
	}
};

class ASFDNextRoomLoader;
class ULevelStreamingDynamic;
class ASFDLevelCore;

DECLARE_EVENT_OneParam(USFDLevelsManager, FRoomSpawnStartedDelegate, const uint8 InSpawningRoomIndex)
DECLARE_EVENT_OneParam(USFDLevelsManager, FRoomSpawnedDelegate, const uint8 InSpawnedRoomIndex)

DECLARE_EVENT_OneParam(USFDLevelsManager, FTransitionRequestReceivedDelegate, const uint8 InTransitionRoomIndex)
DECLARE_EVENT_OneParam(USFDLevelsManager, FTransitionRequestFulfilledDelegate, const uint8 InTransitionRoomIndex)

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

	static int8 GetLoaderIndexByRoomToLoadIndex(const uint8 InRoomIndex
												, const uint8 InRoomToLoadIndex
												, const uint8*const *const InIncidenceMatrix
												, const uint8 InConnectionsAmount
												, const uint8 InRoomsAmount);
	
	static void GetAllConnectionsForRoom(TArray<uint8>& OutConnectedRoomsIndices
												, const uint8 InRoomIndex
												, const uint8*const *const InIncidenceMatrix
												, const uint8 InConnectionsAmount
												, const uint8 InRoomsAmount);
	
	static void InitAdjacency(uint8**& OutAdjacencyMatrixPtrPtr, const uint8 InDimension);
	
	// Find the minimum spanning tree and fill an Incidence Matrix
	static void FindMST(uint8**& OutIncidenceMatrixPtrPtr
						, const uint8*const *const InAdjacencyMatrixPtrPtr
						, const uint8 InAdjacencyMatrixDimension);
	
	static const FSFDRoomNode* const BuildTree(const uint8 InRoomIndex
												, const FSFDRoomNode* ParentRoomNode
												, const uint8* const* const InIncidenceMatrix
												, const uint8 InRoomsAmount
												, TArray<FSFDRoomNode*>* OutRoomsArray = nullptr);
	
	static void ClearMatrix(uint8**& InAdjacencyMatrixPtrPtr, const uint8 InDimensionY);
	static void PrintMatrix(const uint8 * const * const InMatrix, const uint8 InSizeX, const uint8 InSizeY, const FString& InTitle);
	
	FORCEINLINE int8 GetPreviousRoomIndex() const;
	FORCEINLINE int8 GetCurrentRoomIndex() const;
	FORCEINLINE int8 GetPendingRoomIndex() const;

	FORCEINLINE uint8 GetRoomsAmount() const;
	FORCEINLINE uint8 GetConnectionsAmount() const;
	FORCEINLINE uint8 GetStartRoomIndex() const;

	FORCEINLINE const uint8*const *const GetIncidenceMatrix() const;
	FORCEINLINE const FSFDRoomTransitionData& GetRoomTransitionData() const;

	FORCEINLINE FRoomSpawnStartedDelegate& GetOnRoomSpawnStartedDelegate();
	FORCEINLINE FRoomSpawnedDelegate& GetOnRoomSpawnedDelegate();

	FORCEINLINE FTransitionRequestReceivedDelegate& GetOnTransitionRequestReceivedDelegate();
	FORCEINLINE FTransitionRequestFulfilledDelegate& GetOnTransitionRequestFulfilledDelegate();
	
	FORCEINLINE const FSFDRoomNode* GetStartRoomNode() const;
	FORCEINLINE uint8 GetPreviousRoomIndex(const uint8 InRoomIndex) const;
	
	void SpawnStartRoom();

	bool RequestTransitionToTheNextRoom(const uint8 InRequestedRoomIndex, const uint8 InFromRoomLoader);

	void GetAllConnectionsForRoom(TArray<uint8>& OutConnectedRoomsIndices, const uint8 InRoomIndex) const;
	int8 GetLoaderIndexByRoomToLoadIndex(const uint8 InRoomIndex, const uint8 InRoomToLoadIndex) const;

	const FTransform& GetRoomCachedTransform(const uint8 InRoomIndex, const ASFDLevelCore* InCurrentRoom) const;

private:
	
	bool SpawnRoom(const TSoftObjectPtr<UWorld>& InLevelClass, const FVector& InLevelLocation, const FRotator& InLevelRotation);

	void PrepareNextRoom();
	void ClearPreviousRoom();
		
	UFUNCTION()
	void OnNextLevelLoaded();	
	UFUNCTION()
	void OnNextLevelShown();

	void TransportPlayerToLevel(const uint8 InLevelIndex);
	
	UFUNCTION()
	void OnPlayerTransportationFinished();

private:
	
	UPROPERTY(Transient)
	mutable TMap<uint8, FTransform> CachedRoomsTransforms;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess))
	TSoftObjectPtr<UWorld> RoomInstanceTemplate;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess))
	float TransitionBetweenRoomsCameraBlendTime = 3.0f;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess))
	uint8 RoomIndexToStart = 0;
	
	UPROPERTY(Transient)
	ULevelStreamingDynamic* CurrentRoomDynamicInstance;
	
	UPROPERTY(Transient)
	ULevelStreamingDynamic* PendingRoomDynamicInstance;
		
	uint8** AdjacencyMatrix;
	uint8** IncidenceMatrix;
	//const FSFDRoomNode* RoomsTree; 
	TArray<FSFDRoomNode*> RoomsTree;
	
	uint8 RoomsAmount;
	uint8 ConnectionsAmount;

	FSFDRoomTransitionData RoomTransitionData;
		
	int8 PreviousRoomIndex = INDEX_NONE;
	//@TODO temp, should be moved into custom ULevelStreamingDynamic
	int8 CurrentRoomIndex = INDEX_NONE;
	//@TODO temp, should be moved into custom ULevelStreamingDynamic
	int8 PendingRoomIndex = INDEX_NONE;

	FRoomSpawnStartedDelegate OnRoomSpawnStarted;
	FRoomSpawnedDelegate OnRoomSpawned;

	FTransitionRequestReceivedDelegate OnTransitionRequestReceived;
	FTransitionRequestFulfilledDelegate OnTransitionRequestFulfilled;
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

FORCEINLINE const FSFDRoomTransitionData& USFDLevelsManager::GetRoomTransitionData() const
{
	return RoomTransitionData;
};

FORCEINLINE FRoomSpawnStartedDelegate& USFDLevelsManager::GetOnRoomSpawnStartedDelegate()
{
	return OnRoomSpawnStarted;
}

FORCEINLINE FRoomSpawnedDelegate& USFDLevelsManager::GetOnRoomSpawnedDelegate()
{
	return OnRoomSpawned;
}

FORCEINLINE FTransitionRequestReceivedDelegate& USFDLevelsManager::GetOnTransitionRequestReceivedDelegate()
{
	return OnTransitionRequestReceived;
		
}

FORCEINLINE FTransitionRequestFulfilledDelegate& USFDLevelsManager::GetOnTransitionRequestFulfilledDelegate()
{
	return OnTransitionRequestFulfilled;
}

FORCEINLINE uint8 USFDLevelsManager::GetStartRoomIndex() const
{
	return RoomIndexToStart;
}

FORCEINLINE const FSFDRoomNode* USFDLevelsManager::GetStartRoomNode() const
{
	if(RoomsTree.IsValidIndex(RoomIndexToStart))
	{
		return RoomsTree[RoomIndexToStart];
	}
	
	return nullptr;
}

FORCEINLINE uint8 USFDLevelsManager::GetPreviousRoomIndex(const uint8 InRoomIndex) const
{
	if(RoomsTree.IsValidIndex(InRoomIndex)
		&& RoomsTree[InRoomIndex]->PreviousRoom != nullptr)
	{
		return RoomsTree[InRoomIndex]->PreviousRoom->RoomIndex;
	}
	
	return 0xFFu;
}
