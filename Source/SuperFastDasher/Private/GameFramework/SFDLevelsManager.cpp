// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/SFDLevelsManager.h"
#include "GameFramework/SFDNextRoomLoader.h"
#include "SuperFastDasher/SuperFastDasher.h"

#include "Engine/LevelStreamingDynamic.h"
#include "GameFramework/SFDLevelCore.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"



uint8 USFDLevelsManager::LevelsAmount = 10;

USFDLevelsManager::USFDLevelsManager()
{
	InitAdjacency();
	FindMST();

	PrintMatrix(AdjacencyMatrix, LevelsAmount, LevelsAmount, "Adjacency Matrix");
	PrintMatrix(IncidenceMatrix, LevelsAmount - 1, LevelsAmount, "Incidence Matrix");
}

USFDLevelsManager::~USFDLevelsManager()
{
	if(AdjacencyMatrix != nullptr)
    {
    	for(uint8 i = 0; i < LevelsAmount; ++i)
    	{
    		delete[] AdjacencyMatrix[i];
    	}
    	delete[] AdjacencyMatrix;
    }

	if(IncidenceMatrix != nullptr)
	{
		for(uint8 i = 0; i < LevelsAmount - 1; ++i)
		{
			delete[] IncidenceMatrix[i];
		}
		delete[] IncidenceMatrix;
	}
}

void USFDLevelsManager::InitAdjacency()
{
	if(AdjacencyMatrix != nullptr)
	{
		for(uint8 i = 0; i < LevelsAmount; ++i)
		{
			delete[] AdjacencyMatrix[i];
		}
		delete[] AdjacencyMatrix;
	}
	
	AdjacencyMatrix = new uint8*[LevelsAmount];
    for(uint8 i = 0; i < LevelsAmount; ++i)
    {
    	AdjacencyMatrix[i] = new uint8[LevelsAmount];
    }

	for(uint8 i = 0; i < LevelsAmount; ++i)
	{
		for(uint8 j = i; j < LevelsAmount; ++j)
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
	
	if(IncidenceMatrix != nullptr)
	{
		for(uint8 i = 0; i < LevelsAmount - 1; ++i)
		{
			delete[] IncidenceMatrix[i];
		}
		delete[] IncidenceMatrix;
	}
	
	const uint8 EdgesAmount = LevelsAmount - 1; 
	
	TArray<uint8> Tree;
	Tree.Reserve(LevelsAmount);
	Tree.Add(FMath::RandRange(0, LevelsAmount - 1));

	TArray<TPair<uint8, uint8>> Edges;
	Edges.Reserve(EdgesAmount);
	
	while(Tree.Num() < LevelsAmount)
	{
		TPair<int8, int8> CheapestEdge = { INDEX_NONE, INDEX_NONE };
		uint8 CheapestEdgeWeight = MAX_uint8;

		for(uint8 i = 0; i < Tree.Num(); ++i)
		{
			const uint8 SearchedVertex = Tree[i];
			for(uint8 j = 0; j < LevelsAmount; ++j)
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
	
	IncidenceMatrix = new uint8*[LevelsAmount];
	for(uint8 i = 0; i < LevelsAmount; ++i)
	{
		IncidenceMatrix[i] = new uint8[EdgesAmount];
	}

	for(uint8 i = 0; i < LevelsAmount; ++i)
	{
		for(uint8 j = 0; j < EdgesAmount; ++j)
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
	PrintMatrix(AdjacencyMatrix, LevelsAmount, LevelsAmount, "Adjacency Matrix");
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
	PrintMatrix(IncidenceMatrix, LevelsAmount - 1, LevelsAmount, "Incidence Matrix");
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
	PendingLevelIndex = LevelIndexToStart;
	CurrentLevelIndex = INDEX_NONE;
	
	SpawnLevel(LevelInstance, FVector::ZeroVector, FRotator::ZeroRotator);
}

void USFDLevelsManager::SpawnNextRoom(const ASFDNextRoomLoader* InFromRoomLoader)
{
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

	PendingLevelIndex = RoomToLoadIndex;
	
	SpawnLevel(LevelInstance, InNextLevelTransform.GetLocation(), InNextLevelTransform.GetRotation().Rotator());	
}

void USFDLevelsManager::GetAllConnectionsForRoom(TArray<uint8>& OutConnectedRoomsIndices, const uint8 InRoomIndex) const
{
	OutConnectedRoomsIndices.Reset(4);

	const uint8 EdgesAmount = LevelsAmount - 1;
	
	for(uint8 i = 0; i < EdgesAmount; ++i)
	{
		const bool bIsConnectionExists = static_cast<bool>(IncidenceMatrix[i][InRoomIndex]);
		if(bIsConnectionExists)
		{
			for(uint8 j = 0; j < LevelsAmount; ++j)
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
}

void USFDLevelsManager::SpawnLevel(const TSoftObjectPtr<UWorld>& InLevelClass, const FVector& InLevelLocation, const FRotator& InLevelRotation)
{
	if(IsValid(PendingLevelDynamicInstance))
	{
		return;
		
		// PendingLevelDynamicInstance->SetShouldBeLoaded(false);
		// PendingLevelDynamicInstance->SetShouldBeVisible(false);
		// PendingLevelDynamicInstance->SetIsRequestingUnloadAndRemoval(true);
		//
		// PendingLevelDynamicInstance->OnLevelShown.RemoveDynamic(this, &USFDLevelsManager::OnNextLevelLoaded);
		//
		// PendingLevelDynamicInstance = nullptr;
	}

	UWorld* World = GetWorld();
	if(!ensureAlways(IsValid(World)))
	{
		return;
	}
	
	bool bIsSucceed = false;
	PendingLevelDynamicInstance = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(World, LevelInstance, FVector::ZeroVector, FRotator::ZeroRotator, bIsSucceed);
	if(!ensureAlways(bIsSucceed)
		|| !ensureAlways(IsValid(PendingLevelDynamicInstance)))
	{
		return;
	}
	
	//PendingLevelDynamicInstance->OnLevelLoaded.AddUniqueDynamic(this, &USFDLevelsManager::OnNextLevelLoaded);
	PendingLevelDynamicInstance->OnLevelShown.AddUniqueDynamic(this, &USFDLevelsManager::OnNextLevelLoaded);
}

void USFDLevelsManager::OnNextLevelLoaded()
{
	if(!ensureAlways(IsValid(PendingLevelDynamicInstance)))
	{
		return;
	}

	//PendingLevelDynamicInstance->OnLevelLoaded.RemoveDynamic(this, &USFDLevelsManager::OnNextLevelLoaded);
	PendingLevelDynamicInstance->OnLevelShown.RemoveDynamic(this, &USFDLevelsManager::OnNextLevelLoaded);

	PendingLevelDynamicInstance->SetShouldBeVisible(true);
	PendingLevelDynamicInstance->SetShouldBeLoaded(true);
	
	const ULevel* InNextLevel = PendingLevelDynamicInstance->GetLoadedLevel();

	ASFDLevelCore* NextLevelCore = SFD::GetLevelCore(InNextLevel);
	if(!ensureAlways(IsValid(NextLevelCore)))
	{
		return;
	}

	NextLevelCore->SetRoomIndex(PendingLevelIndex);
	
	TransportPlayer(NextLevelCore->GetPlayerStart());
	
	if(IsValid(CurrentLevelDynamicInstance))
    {
    	CurrentLevelDynamicInstance->SetShouldBeLoaded(false);
    	CurrentLevelDynamicInstance->SetShouldBeVisible(false);
    	CurrentLevelDynamicInstance->SetIsRequestingUnloadAndRemoval(true);
		
		CurrentLevelDynamicInstance = nullptr;
    }

	CurrentLevelIndex = PendingLevelIndex;
	PendingLevelIndex = INDEX_NONE;
	
	CurrentLevelDynamicInstance = PendingLevelDynamicInstance;
	PendingLevelDynamicInstance = nullptr;
}

void USFDLevelsManager::TransportPlayer(const APlayerStart* InPlayerStart)
{
	if(!ensureAlways(IsValid(InPlayerStart)))
	{
		return;
	}
	
	const UWorld* World = GetWorld();
	if(!ensureAlways(IsValid(World)))
	{
		return;
	}
	
	const APlayerController* PlayerController = World->GetFirstPlayerController();
	if(!ensureAlways(IsValid(PlayerController)))
	{
		return;
	}

	PlayerController->GetPawn()->SetActorLocation(InPlayerStart->GetActorLocation());
	PlayerController->GetPawn()->SetActorRotation(InPlayerStart->GetActorRotation());
}

