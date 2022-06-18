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
	RoomsAmount = 10;
	ConnectionsAmount = RoomsAmount - 1;
	
	InitAdjacency();
	FindMST();

	PrintMatrix(AdjacencyMatrix, RoomsAmount, RoomsAmount, "Adjacency Matrix");
	PrintMatrix(IncidenceMatrix, ConnectionsAmount, RoomsAmount, "Incidence Matrix");
}

USFDLevelsManager::~USFDLevelsManager()
{
	ClearAdjacencyMatrix();
	ClearIncidenceMatrix();
}

void USFDLevelsManager::ClearAdjacencyMatrix()
{
	if(AdjacencyMatrix != nullptr)
	{
		for(uint8 i = 0; i < RoomsAmount; ++i)
		{
			delete[] AdjacencyMatrix[i];
		}
		delete[] AdjacencyMatrix;
	}
}

void USFDLevelsManager::ClearIncidenceMatrix()
{
	if(IncidenceMatrix != nullptr)
	{
		for(uint8 i = 0; i < RoomsAmount - 1; ++i)
		{
			delete[] IncidenceMatrix[i];
		}
		delete[] IncidenceMatrix;
	}
}

void USFDLevelsManager::InitAdjacency()
{
	ClearAdjacencyMatrix();
	
	AdjacencyMatrix = new uint8*[RoomsAmount];
    for(uint8 i = 0; i < RoomsAmount; ++i)
    {
    	AdjacencyMatrix[i] = new uint8[RoomsAmount];
    }

	for(uint8 i = 0; i < RoomsAmount; ++i)
	{
		for(uint8 j = i; j < RoomsAmount; ++j)
		{
			if(i == j)
			{
				AdjacencyMatrix[j][i] = 0;
				continue;
			}
			
			const uint8 Weight = FMath::RandRange(1, 20);
			AdjacencyMatrix[j][i] = Weight;
			AdjacencyMatrix[i][j] = Weight;
		}
	}
}

void USFDLevelsManager::FindMST()
{
	if(AdjacencyMatrix == nullptr)
	{
		return;
	}
	
	ClearIncidenceMatrix();

	TArray<uint8> Tree;
	Tree.Reserve(RoomsAmount);
	Tree.Add(FMath::RandRange(0, RoomsAmount - 1));

	TArray<TPair<uint8, uint8>> Edges;
	Edges.Reserve(ConnectionsAmount);
	
	while(Tree.Num() < RoomsAmount)
	{
		TPair<int8, int8> CheapestEdge = { INDEX_NONE, INDEX_NONE };
		uint8 CheapestEdgeWeight = MAX_uint8;

		for(uint8 i = 0; i < Tree.Num(); ++i)
		{
			const uint8 SearchedVertex = Tree[i];
			for(uint8 j = 0; j < RoomsAmount; ++j)
			{
				if(Tree.Contains(j))
				{
					continue;
				}
			
				const uint8 CurrentWeight = AdjacencyMatrix[j][SearchedVertex];
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
	
	IncidenceMatrix = new uint8*[RoomsAmount];
	for(uint8 i = 0; i < RoomsAmount; ++i)
	{
		IncidenceMatrix[i] = new uint8[ConnectionsAmount];
	}

	for(uint8 i = 0; i < RoomsAmount; ++i)
	{
		for(uint8 j = 0; j < ConnectionsAmount; ++j)
		{
			IncidenceMatrix[j][i] = 0;
		}
	}
	
	for(uint8 i = 0; i < Edges.Num(); ++i)
	{
		const TPair<uint8, uint8>& Edge = Edges[i];
		
		IncidenceMatrix[i][Edge.Key] = 1;
		IncidenceMatrix[i][Edge.Value] = 1;
	}
}

void USFDLevelsManager::PrintAdjacencyMatrix_DEBUG()
{
	InitAdjacency();
	PrintMatrix(AdjacencyMatrix, RoomsAmount, RoomsAmount, "Adjacency Matrix");

	ClearAdjacencyMatrix();
}

void USFDLevelsManager::PrintIncidenceMatrix_DEBUG()
{
	PrintAdjacencyMatrix_DEBUG();
	
	if(AdjacencyMatrix == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No Adjacency Matrix Provided"));
		return;
	}
	
	FindMST();
	PrintMatrix(IncidenceMatrix, RoomsAmount - 1, RoomsAmount, "Incidence Matrix");

	ClearAdjacencyMatrix();
	ClearIncidenceMatrix();
}

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
	OutConnectedRoomsIndices.Reset(4);

	for(uint8 i = 0; i < ConnectionsAmount; ++i)
	{
		const bool bIsConnectionExists = static_cast<bool>(IncidenceMatrix[i][InRoomIndex]);
		if(bIsConnectionExists)
		{
			for(uint8 j = 0; j < RoomsAmount; ++j)
			{
				if(j == InRoomIndex)
				{
					continue;
				}
				
				const bool bIsAlsoConnected = static_cast<bool>(IncidenceMatrix[i][j]);
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

