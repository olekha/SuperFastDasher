// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/SFDLevelsManager.h"
#include "GameFramework/SFDNextRoomLoader.h"
#include "SuperFastDasher/SuperFastDasher.h"

#include "Engine/LevelStreamingDynamic.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SFDLevelCore.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "Player/SFDCharacter.h"

USFDLevelsManager::USFDLevelsManager()
{
	RoomsAmount = 12;
	ConnectionsAmount = RoomsAmount - 1;
	
	USFDLevelsManager::InitAdjacency(AdjacencyMatrix, RoomsAmount);
	USFDLevelsManager::FindMST(IncidenceMatrix, AdjacencyMatrix, RoomsAmount);

	USFDLevelsManager::PrintMatrix(AdjacencyMatrix, RoomsAmount, RoomsAmount, "Adjacency Matrix");
	USFDLevelsManager::PrintMatrix(IncidenceMatrix, ConnectionsAmount, RoomsAmount, "Incidence Matrix");
}

USFDLevelsManager::~USFDLevelsManager()
{
	USFDLevelsManager::ClearMatrix(AdjacencyMatrix, RoomsAmount);
	USFDLevelsManager::ClearMatrix(IncidenceMatrix, RoomsAmount);
}

void USFDLevelsManager::ClearMatrix(uint8**& InMatrixPtrPtr, const uint8 InDimensionY)
{
	if(InMatrixPtrPtr == nullptr
		|| (*InMatrixPtrPtr) == nullptr)
	{
		return;
	}
	
    for(uint8 i = 0; i < InDimensionY; ++i)
    {
    	delete[] InMatrixPtrPtr[i];
    }
	
    delete[] InMatrixPtrPtr;
}

void USFDLevelsManager::InitAdjacency(uint8**& OutAdjacencyMatrixPtrPtr, const uint8 InDimension)
{
	USFDLevelsManager::ClearMatrix(OutAdjacencyMatrixPtrPtr, InDimension);
	
	OutAdjacencyMatrixPtrPtr = new uint8*[InDimension];
    for(uint8 i = 0; i < InDimension; ++i)
    {
    	OutAdjacencyMatrixPtrPtr[i] = new uint8[InDimension];
    }

	for(uint8 i = 0; i < InDimension; ++i)
	{
		for(uint8 j = i; j < InDimension; ++j)
		{
			if(i == j)
			{
				OutAdjacencyMatrixPtrPtr[j][i] = 0;
				continue;
			}
			
			const uint8 Weight = FMath::RandRange(1, 20);
			OutAdjacencyMatrixPtrPtr[j][i] = Weight;
			OutAdjacencyMatrixPtrPtr[i][j] = Weight;
		}
	}
}

void USFDLevelsManager::FindMST(uint8**& OutIncidenceMatrixPtrPtr, const uint8* const* const InAdjacencyMatrixPtrPtr, const uint8 InAdjacencyMatrixDimension)
{
	if(InAdjacencyMatrixPtrPtr == nullptr
		|| (*InAdjacencyMatrixPtrPtr) == nullptr)
	{
		return;
	}
	
	TArray<uint8> Tree;
	Tree.Reserve(InAdjacencyMatrixDimension);
	Tree.Add(FMath::RandRange(0, InAdjacencyMatrixDimension - 1));

	const uint8 ConnectionsAmount = InAdjacencyMatrixDimension - 1;
	
	TArray<TPair<uint8, uint8>> Edges;
	Edges.Reserve(ConnectionsAmount);
	
	while(Tree.Num() < InAdjacencyMatrixDimension)
	{
		TPair<int8, int8> CheapestEdge = { INDEX_NONE, INDEX_NONE };
		uint8 CheapestEdgeWeight = MAX_uint8;

		for(uint8 i = 0; i < Tree.Num(); ++i)
		{
			const uint8 SearchedVertex = Tree[i];
			for(uint8 j = 0; j < InAdjacencyMatrixDimension; ++j)
			{
				if(Tree.Contains(j))
				{
					continue;
				}
			
				const uint8 CurrentWeight = InAdjacencyMatrixPtrPtr[j][SearchedVertex];
				if(CurrentWeight == 0)
				{
					continue;
				}
				
				if(CurrentWeight < CheapestEdgeWeight)
				{
					CheapestEdgeWeight = CurrentWeight;
					CheapestEdge = {j, SearchedVertex };
				}
			}
		}

		if(CheapestEdge.Key != INDEX_NONE && CheapestEdge.Value != INDEX_NONE)
		{
			Tree.Add(CheapestEdge.Key);
			Edges.Add(CheapestEdge);
		}
	}

	USFDLevelsManager::ClearMatrix(OutIncidenceMatrixPtrPtr, InAdjacencyMatrixDimension);
	
	OutIncidenceMatrixPtrPtr = new uint8*[InAdjacencyMatrixDimension];
	for(uint8 i = 0; i < InAdjacencyMatrixDimension; ++i)
	{
		OutIncidenceMatrixPtrPtr[i] = new uint8[ConnectionsAmount];
	}

	for(uint8 i = 0; i < InAdjacencyMatrixDimension; ++i)
	{
		for(uint8 j = 0; j < ConnectionsAmount; ++j)
		{
			OutIncidenceMatrixPtrPtr[j][i] = 0;
		}
	}
	
	for(uint8 i = 0; i < Edges.Num(); ++i)
	{
		const TPair<uint8, uint8>& Edge = Edges[i];
		
		OutIncidenceMatrixPtrPtr[i][Edge.Key] = 1;
		OutIncidenceMatrixPtrPtr[i][Edge.Value] = 1;
	}
}

void USFDLevelsManager::ArrangeRooms()
{
	TArray<uint8> Connections;
	GetAllConnectionsForRoom(Connections, RoomIndexToStart);
}

#if WITH_EDITOR
void USFDLevelsManager::PrintAdjacencyMatrix_DEBUG()
{
	uint8** DebugAdjacencyMatrix = nullptr;
	USFDLevelsManager::InitAdjacency(DebugAdjacencyMatrix, RoomsAmount);
	USFDLevelsManager::PrintMatrix(DebugAdjacencyMatrix, RoomsAmount, RoomsAmount, "Adjacency Matrix");

	USFDLevelsManager::ClearMatrix(DebugAdjacencyMatrix, RoomsAmount);
}

void USFDLevelsManager::PrintIncidenceMatrix_DEBUG()
{
	uint8** DebugAdjacencyMatrix = nullptr;
	USFDLevelsManager::InitAdjacency(DebugAdjacencyMatrix, RoomsAmount);
	USFDLevelsManager::PrintMatrix(DebugAdjacencyMatrix, RoomsAmount, RoomsAmount, "Adjacency Matrix");

	uint8** DebugIncidenceMatrix = nullptr;
	USFDLevelsManager::FindMST(DebugIncidenceMatrix, DebugAdjacencyMatrix, RoomsAmount);
	USFDLevelsManager::PrintMatrix(DebugIncidenceMatrix, RoomsAmount - 1, RoomsAmount, "Incidence Matrix");

	USFDLevelsManager::ClearMatrix(DebugAdjacencyMatrix, RoomsAmount);
	USFDLevelsManager::ClearMatrix(DebugIncidenceMatrix, RoomsAmount);
}
#endif

void USFDLevelsManager::PrintMatrix(const uint8* const* const InMatrix, const uint8 InSizeX, const uint8 InSizeY, const FString& InTitle)
{
	if(InMatrix == nullptr)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("------------------------%s-----------------------"), *InTitle);
	
	FString RowHeaderString = "     ";
	for(uint8 i = 0; i < InSizeX; ++i)
	{
		//RowHeaderString += FString::Printf(TEXT("e%i  "),i);
		FText VertexIndexText = UKismetTextLibrary::Conv_IntToText(i, false, false, 2);
		RowHeaderString += (VertexIndexText.ToString() + "  ");
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *RowHeaderString);
	
	for(uint8 i = 0; i < InSizeY; ++i)
	{
		FText VertexIndexText = UKismetTextLibrary::Conv_IntToText(i, false, false, 2);
		FString RowString = FString::Printf(TEXT("%s [ "), *(VertexIndexText.ToString()));

		for(uint8 j = 0; j < InSizeX; ++j)
		{
			const uint8 Connection = InMatrix[j][i];

			FText ConnectionText = UKismetTextLibrary::Conv_IntToText(Connection, false, false, 2);
    		
			RowString += ConnectionText.ToString();

			if(j != InSizeX - 1)
			{
				RowString += ", ";
			}
		}
		RowString += " ]";
    	
		UE_LOG(LogTemp, Warning, TEXT("%s"), *RowString);
	}
}

void USFDLevelsManager::SpawnStartRoom()
{
	if(IsValid(PendingRoomDynamicInstance))
	{
		return;
	}
	
	PendingRoomIndex = RoomIndexToStart;
	CurrentRoomIndex = INDEX_NONE;
	
	SpawnRoom(RoomInstanceTemplate, FVector::ZeroVector, FRotator::ZeroRotator);
}

void USFDLevelsManager::SpawnNextRoom(const ASFDNextRoomLoader* InFromRoomLoader)
{
	if(IsValid(PendingRoomDynamicInstance))
	{
		return;
	}
	
	if(!ensureAlways(IsValid(InFromRoomLoader)))
	{
		return;
	}

	const ULevel* RoomLoaderLevel = InFromRoomLoader->GetLevel();
	if(!ensureAlways(IsValid(RoomLoaderLevel)))
	{
		return;
	}

	const ASFDLevelCore* CurrentLevelCore = SFD::GetLevelCore(RoomLoaderLevel);
	if(!ensureAlways(IsValid(RoomLoaderLevel)))
	{
		return;
	}
	
	const uint8 RoomToLoadIndex = InFromRoomLoader->GetRoomToLoadIndex();
	const uint8 LocalIndex = InFromRoomLoader->GetLocalIndex();
	const FTransform& InNextLevelTransform = CurrentLevelCore->GetNextRoomTransformByLocalIndex(LocalIndex);

	PendingRoomIndex = RoomToLoadIndex;
	
	SpawnRoom(RoomInstanceTemplate, InNextLevelTransform.GetLocation(), InNextLevelTransform.GetRotation().Rotator());	
}

void USFDLevelsManager::GetAllConnectionsForRoom(TArray<uint8>& OutConnectedRoomsIndices, const uint8 InRoomIndex) const
{
	USFDLevelsManager::GetAllConnectionsForRoom(OutConnectedRoomsIndices, InRoomIndex, IncidenceMatrix, ConnectionsAmount, RoomsAmount);
}

void USFDLevelsManager::GetAllConnectionsForRoom(TArray<uint8>& OutConnectedRoomsIndices, const uint8 InRoomIndex,
	const uint8* const* const InIncidenceMatrix, const uint8 InConnectionsAmount, const uint8 InRoomsAmount)
{
	OutConnectedRoomsIndices.Reset(4);
	
	if(InIncidenceMatrix == nullptr
		|| (*InIncidenceMatrix) == nullptr)
	{
		return;
	}

	for(uint8 i = 0; i < InConnectionsAmount; ++i)
	{
		const bool bIsConnectionExists = static_cast<bool>(InIncidenceMatrix[i][InRoomIndex]);
		if(bIsConnectionExists)
		{
			for(uint8 j = 0; j < InRoomsAmount; ++j)
			{
				if(j == InRoomIndex)
				{
					continue;
				}
				
				const bool bIsAlsoConnected = static_cast<bool>(InIncidenceMatrix[i][j]);
				if(bIsAlsoConnected)
				{
					OutConnectedRoomsIndices.Add(j);
				}
			}
		}
	}
	
	OutConnectedRoomsIndices.Sort();
}

void USFDLevelsManager::SpawnRoom(const TSoftObjectPtr<UWorld>& InLevelClass, const FVector& InLevelLocation, const FRotator& InLevelRotation)
{
	if(IsValid(PendingRoomDynamicInstance))
	{
		return;
	}

	UWorld* World = GetWorld();
	if(!ensureAlways(IsValid(World)))
	{
		return;
	}
	
	bool bIsSucceed = false;
	PendingRoomDynamicInstance = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(World, RoomInstanceTemplate, FVector::ZeroVector, FRotator::ZeroRotator, bIsSucceed);
	if(!ensureAlways(bIsSucceed)
		|| !ensureAlways(IsValid(PendingRoomDynamicInstance)))
	{
		return;
	}
	
	//PendingLevelDynamicInstance->OnLevelLoaded.AddUniqueDynamic(this, &USFDLevelsManager::OnNextLevelLoaded);
	PendingRoomDynamicInstance->OnLevelShown.AddUniqueDynamic(this, &USFDLevelsManager::OnNextLevelLoaded);
}

void USFDLevelsManager::OnNextLevelLoaded()
{
	if(!ensureAlways(IsValid(PendingRoomDynamicInstance)))
	{
		return;
	}

	//PendingLevelDynamicInstance->OnLevelLoaded.RemoveDynamic(this, &USFDLevelsManager::OnNextLevelLoaded);
	PendingRoomDynamicInstance->OnLevelShown.RemoveDynamic(this, &USFDLevelsManager::OnNextLevelLoaded);

	PrepareNextRoom();

	TransportPlayerToNextLevel();

	ClearPreviousRoom();

	PreviousRoomIndex = CurrentRoomIndex;
	CurrentRoomIndex = PendingRoomIndex;
	PendingRoomIndex = INDEX_NONE;
	
	CurrentRoomDynamicInstance = PendingRoomDynamicInstance;
	PendingRoomDynamicInstance = nullptr;
}

void USFDLevelsManager::PrepareNextRoom()
{
	if(!ensureAlways(IsValid(PendingRoomDynamicInstance))
		|| !ensureAlways(PendingRoomIndex != INDEX_NONE))
	{
		return;
	}
	
	PendingRoomDynamicInstance->SetShouldBeVisible(true);
	PendingRoomDynamicInstance->SetShouldBeLoaded(true);

	const ULevel* InNextLevel = PendingRoomDynamicInstance->GetLoadedLevel();

	ASFDLevelCore* NextLevelCore = SFD::GetLevelCore(InNextLevel);
	if(!ensureAlways(IsValid(NextLevelCore)))
	{
		return;
	}

	NextLevelCore->SetRoomIndex(PendingRoomIndex);
}

void USFDLevelsManager::ClearPreviousRoom()
{
	if(IsValid(CurrentRoomDynamicInstance))
	{
		CurrentRoomDynamicInstance->SetShouldBeLoaded(false);
		CurrentRoomDynamicInstance->SetShouldBeVisible(false);
		CurrentRoomDynamicInstance->SetIsRequestingUnloadAndRemoval(true);
	}
}

void USFDLevelsManager::TransportPlayerToNextLevel()
{
	if(!ensureAlways(IsValid(PendingRoomDynamicInstance)))
	{
		return;
	}
	
	const UWorld* World = GetWorld();
	if(!ensureAlways(IsValid(World)))
	{
		return;
	}
	
	const APlayerController* PlayerController = World->GetFirstPlayerController();
	if(!ensureAlways(IsValid(PlayerController))
		|| !ensureAlways(IsValid(PlayerController->GetPawn())))
	{
		return;
	}

	const ULevel* NextRoomLevel = PendingRoomDynamicInstance->GetLoadedLevel();

	const ASFDLevelCore* NextLevelCore = SFD::GetLevelCore(NextRoomLevel);
	if(!ensureAlways(IsValid(NextLevelCore)))
	{
		return;
	}
	
	FVector LocationToSpawnPlayer = FVector::ZeroVector;
	FRotator RotatorToSpawnPlayer = FRotator::ZeroRotator;
	
	if(CurrentRoomIndex != INDEX_NONE)
	{
		ASFDNextRoomLoader* CurrentRoomLoaderInsidePendingRoom = NextLevelCore->GetNextRoomLoaderByRoomIndex(CurrentRoomIndex);
		if(!ensureAlways(IsValid(CurrentRoomLoaderInsidePendingRoom)))
		{
			return;
		}
		
		CurrentRoomLoaderInsidePendingRoom->BlockSpawnTillPlayerStepOut();
		
		LocationToSpawnPlayer = CurrentRoomLoaderInsidePendingRoom->GetLocationToSpawnPlayer();
		RotatorToSpawnPlayer = CurrentRoomLoaderInsidePendingRoom->GetRotationToSpawnPlayer();
	}
	else
	{
		const APlayerStart* StartToSpawnPlayer = NextLevelCore->GetPlayerStart();  
		if(!ensureAlways(IsValid(StartToSpawnPlayer)))
		{
			return;
		}
		
		LocationToSpawnPlayer = StartToSpawnPlayer->GetActorLocation();
		RotatorToSpawnPlayer = StartToSpawnPlayer->GetActorRotation();
	}
	
	PlayerController->GetPawn()->TeleportTo(LocationToSpawnPlayer, RotatorToSpawnPlayer, false, true);
}
