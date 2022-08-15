// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/SFDLevelsManager.h"
#include "GameFramework/SFDNextRoomLoader.h"
#include "SuperFastDasher/SuperFastDasher.h"
#include "Player/SFDCharacter.h"

#include "Engine/LevelStreamingDynamic.h"
#include "GameFramework/SFDLevelCore.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "GameFramework/PlayerStart.h"

USFDLevelsManager::USFDLevelsManager()
{
	RoomsAmount = 12;
	ConnectionsAmount = RoomsAmount - 1;
	
	USFDLevelsManager::InitAdjacency(AdjacencyMatrix, RoomsAmount);
	USFDLevelsManager::FindMST(IncidenceMatrix, AdjacencyMatrix, RoomsAmount);

	USFDLevelsManager::PrintMatrix(AdjacencyMatrix, RoomsAmount, RoomsAmount, "Adjacency Matrix");
	USFDLevelsManager::PrintMatrix(IncidenceMatrix, ConnectionsAmount, RoomsAmount, "Incidence Matrix");

	RoomsTree.Empty();
	RoomsTree.InsertDefaulted(0, RoomsAmount);
	
	USFDLevelsManager::BuildTree(RoomIndexToStart, nullptr, IncidenceMatrix, RoomsAmount, &RoomsTree);
}

USFDLevelsManager::~USFDLevelsManager()
{
	USFDLevelsManager::ClearMatrix(AdjacencyMatrix, RoomsAmount);
	USFDLevelsManager::ClearMatrix(IncidenceMatrix, RoomsAmount);

	if(RoomsTree.IsValidIndex(RoomIndexToStart)
		&& RoomsTree[RoomIndexToStart] != nullptr)
	{
		delete RoomsTree[RoomIndexToStart];
	}
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

const FSFDRoomNode* const USFDLevelsManager::BuildTree(const uint8 InRoomIndex
														, const FSFDRoomNode* ParentRoomNode
														, const uint8* const* const InIncidenceMatrix
														, const uint8 InRoomsAmount
														, TArray<FSFDRoomNode*>* OutRoomsArray /*= nullptr*/)
{
	TArray<uint8> Connections;
	USFDLevelsManager::GetAllConnectionsForRoom(Connections, InRoomIndex, InIncidenceMatrix, InRoomsAmount - 1, InRoomsAmount);
	if(ParentRoomNode != nullptr)
	{
		Connections.RemoveSingle(ParentRoomNode->RoomIndex);
	}
	
	FSFDRoomNode* Node = new FSFDRoomNode(InRoomIndex, ParentRoomNode, {});

	if(OutRoomsArray != nullptr)
	{
		(*OutRoomsArray)[InRoomIndex] = Node;
	}
	
	for(uint8 i = 0; i < Connections.Num(); ++i)
	{
		Node->NextRooms.Add(USFDLevelsManager::BuildTree(Connections[i], Node, InIncidenceMatrix, InRoomsAmount, OutRoomsArray));
	}

	return Node;
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

void USFDLevelsManager::GetAllConnectionsForRoom(TArray<uint8>& OutConnectedRoomsIndices, const uint8 InRoomIndex) const
{
	USFDLevelsManager::GetAllConnectionsForRoom(OutConnectedRoomsIndices, InRoomIndex, IncidenceMatrix, ConnectionsAmount, RoomsAmount);
}

int8 USFDLevelsManager::GetLoaderIndexByRoomToLoadIndex(const uint8 InRoomIndex, const uint8 InRoomToLoadIndex) const
{
	return USFDLevelsManager::GetLoaderIndexByRoomToLoadIndex(InRoomIndex, InRoomToLoadIndex, IncidenceMatrix, ConnectionsAmount, RoomsAmount);
}

const FTransform& USFDLevelsManager::GetRoomCachedTransform(const uint8 InRoomIndex, const ASFDLevelCore* InCurrentRoom) const
{
	if(const FTransform* CachedTransform = CachedRoomsTransforms.Find(InRoomIndex))
	{
		return *CachedTransform;
	}

	if(!IsValid(InCurrentRoom))
	{
		return CachedRoomsTransforms.Add(InRoomIndex, FTransform::Identity);
	}
	
	const ASFDNextRoomLoader* RoomLoader = InCurrentRoom->GetNextRoomLoaderByRoomIndex(InRoomIndex);
	if(!IsValid(RoomLoader))
	{
		return CachedRoomsTransforms.Add(InRoomIndex, FTransform::Identity);
	}

	const FVector SpawnerLocation = RoomLoader->GetActorLocation();
	const FVector RoomCenter = InCurrentRoom->GetRoomCenterLocation();
	
	FVector NextRoomLocation = RoomCenter + ((SpawnerLocation - RoomCenter).GetSafeNormal() * InCurrentRoom->GetDistanceBetweenRooms());
	
	float RoomZOffset = FMath::RandRange(InCurrentRoom->GetNextRoomsZOffsetMinValue(), InCurrentRoom->GetNextRoomsZOffset());
	RoomZOffset = FMath::Abs(RoomZOffset) < 1.0f ? 1.0f : RoomZOffset;

	const float Sign = FMath::RandBool() ? 1.0f : -1.0f;
	
	NextRoomLocation.Z += RoomZOffset * Sign;
	
	return CachedRoomsTransforms.Add(InRoomIndex, FTransform(NextRoomLocation));
}

int8 USFDLevelsManager::GetLoaderIndexByRoomToLoadIndex(const uint8 InRoomIndex, const uint8 InRoomToLoadIndex,
                                                        const uint8* const* const InIncidenceMatrix, const uint8 InConnectionsAmount, const uint8 InRoomsAmount)
{
	static TArray<uint8> ConnectedRooms;
	USFDLevelsManager::GetAllConnectionsForRoom(ConnectedRooms, InRoomIndex, InIncidenceMatrix, InConnectionsAmount, InRoomsAmount);

	return ConnectedRooms.IndexOfByPredicate([InRoomToLoadIndex](const uint8 InConnectedRoomIndex){ return InRoomToLoadIndex == InConnectedRoomIndex; });
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

void USFDLevelsManager::SpawnStartRoom()
{
	if(IsValid(PendingRoomDynamicInstance))
	{
		return;
	}

	RoomTransitionData.Reset();
	RoomTransitionData.ToRoom = RoomIndexToStart;
	
	PendingRoomIndex = RoomIndexToStart;
	PreviousRoomIndex = INDEX_NONE;
	CurrentRoomIndex = INDEX_NONE;
	
	const FTransform& InNextLevelTransform = GetRoomCachedTransform(RoomIndexToStart, nullptr);

	SpawnRoom(RoomInstanceTemplate, InNextLevelTransform.GetLocation(), InNextLevelTransform.GetRotation().Rotator());

	UE_LOG(LogTemp, Warning, TEXT("Room spawned location: %f, %f, %f")
		, InNextLevelTransform.GetLocation().X
		, InNextLevelTransform.GetLocation().Y
		, InNextLevelTransform.GetLocation().Z);
}

bool USFDLevelsManager::RequestTransitionToTheNextRoom(const uint8 InRequestedRoomIndex, const uint8 InFromRoomLoader)
{
	if(IsValid(PendingRoomDynamicInstance))
	{
		return false;
	}

	RoomTransitionData.Reset();
	RoomTransitionData.FromRoom = CurrentRoomIndex;
	RoomTransitionData.FromRoomLoader = InFromRoomLoader;
	RoomTransitionData.ToRoom = InRequestedRoomIndex;

	if(OnTransitionRequestReceived.IsBound())
	{
		OnTransitionRequestReceived.Broadcast(InRequestedRoomIndex);
	}
	
	const ASFDLevelCore* RequestedRoomCore = SFD::GetLevelCore(this, InRequestedRoomIndex);
	if(IsValid(RequestedRoomCore))
	{
		TransportPlayerToLevel(RoomTransitionData.ToRoom);
		return true;
	}
	
	const ASFDLevelCore* CurrentRoomCore = SFD::GetLevelCore(this, CurrentRoomIndex);
	if(!ensureAlways(IsValid(CurrentRoomCore)))
	{
		return false;
	}

	PendingRoomIndex = InRequestedRoomIndex;
	
	const FTransform& InNextLevelTransform = GetRoomCachedTransform(InRequestedRoomIndex, CurrentRoomCore);

	UE_LOG(LogTemp, Warning, TEXT("Room spawned location: %f, %f, %f")
		, InNextLevelTransform.GetLocation().X
		, InNextLevelTransform.GetLocation().Y
		, InNextLevelTransform.GetLocation().Z);
	
	return SpawnRoom(RoomInstanceTemplate, InNextLevelTransform.GetLocation(), InNextLevelTransform.GetRotation().Rotator());
}

bool USFDLevelsManager::SpawnRoom(const TSoftObjectPtr<UWorld>& InLevelClass, const FVector& InLevelLocation, const FRotator& InLevelRotation)
{
	if(IsValid(PendingRoomDynamicInstance))
	{
		return false;
	}

	UWorld* World = GetWorld();
	if(!ensureAlways(IsValid(World)))
	{
		return false;
	}
	
	bool bIsSucceed = false;
	PendingRoomDynamicInstance = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(World, RoomInstanceTemplate, InLevelLocation, InLevelRotation, bIsSucceed);
	if(!ensureAlways(bIsSucceed)
		|| !ensureAlways(IsValid(PendingRoomDynamicInstance)))
	{
		PendingRoomDynamicInstance = nullptr;
		return false;
	}

	if(OnRoomSpawnStarted.IsBound())
	{
		OnRoomSpawnStarted.Broadcast(PendingRoomIndex);
	}
	
	PendingRoomDynamicInstance->OnLevelLoaded.AddUniqueDynamic(this, &USFDLevelsManager::OnNextLevelLoaded);
	PendingRoomDynamicInstance->OnLevelShown.AddUniqueDynamic(this, &USFDLevelsManager::OnNextLevelShown);

	return true;
}

void USFDLevelsManager::OnNextLevelLoaded()
{
	if(!ensureAlways(IsValid(PendingRoomDynamicInstance)))
	{
		return;
	}
	
	PendingRoomDynamicInstance->OnLevelLoaded.RemoveDynamic(this, &USFDLevelsManager::OnNextLevelLoaded);
}

void USFDLevelsManager::OnNextLevelShown()
{
	if(!ensureAlways(IsValid(PendingRoomDynamicInstance)))
	{
		return;
	}
	
	PendingRoomDynamicInstance->OnLevelShown.RemoveDynamic(this, &USFDLevelsManager::OnNextLevelShown);

	PrepareNextRoom();
	
	if(OnRoomSpawned.IsBound())
	{
		OnRoomSpawned.Broadcast(PendingRoomIndex);
	}
	
	if(RoomTransitionData.IsValid())
	{
		TransportPlayerToLevel(RoomTransitionData.ToRoom);
	}
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

	NextLevelCore->InitializeRoom(PendingRoomIndex);
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

void USFDLevelsManager::TransportPlayerToLevel(const uint8 InLevelIndex)
{
	const ASFDLevelCore* NextLevelCore = SFD::GetLevelCore(this, InLevelIndex);
	if(!ensureAlways(IsValid(NextLevelCore)))
	{
		return;
	}
		
	ASFDCharacter* Character = SFD::GetCharacter(this);
	if(!ensureAlways(IsValid(Character)))
	{
		return;
	}

	if(!Character->GetOnPlayerTeleportedDelegate().IsBound())
	{
		Character->GetOnPlayerTeleportedDelegate().AddUObject(this, &USFDLevelsManager::OnPlayerTransportationFinished);
	}

	if(CurrentRoomIndex != INDEX_NONE)
	{
		const ASFDNextRoomLoader* CurrentRoomLoaderInsidePendingRoom = NextLevelCore->GetNextRoomLoaderByRoomIndex(CurrentRoomIndex);
		if(!ensureAlways(IsValid(CurrentRoomLoaderInsidePendingRoom)))
		{
			return;
		}
		
		const FTransform& TransformToSpawnPlayer = CurrentRoomLoaderInsidePendingRoom->GetTransformToSpawnPlayer();

		Character->TeleportPlayer(TransformToSpawnPlayer, false);
	}
	else
	{
		const APlayerStart* StartToSpawnPlayer = NextLevelCore->GetPlayerStart();  
		if(!ensureAlways(IsValid(StartToSpawnPlayer)))
		{
			return;
		}
		
		const FTransform& TransformToSpawnPlayer = StartToSpawnPlayer->GetActorTransform();

		Character->TeleportPlayer(TransformToSpawnPlayer, true);
	}
}

void USFDLevelsManager::OnPlayerTransportationFinished()
{
	ASFDCharacter* Character = SFD::GetCharacter(this);
	if(!ensureAlways(IsValid(Character)))
	{
		return;
	}

	if(Character->GetOnPlayerTeleportedDelegate().IsBound())
	{
		Character->GetOnPlayerTeleportedDelegate().RemoveAll(this);
	}
	
	APlayerController* PlayerController = SFD::GetPlayerController(this);
	if(!ensureAlways(IsValid(PlayerController)))
	{
		return;
	}

	if(PlayerController->GetViewTarget() != Character)
	{
		FViewTargetTransitionParams param;
		param.BlendTime = TransitionBetweenRoomsCameraBlendTime;
			
		PlayerController->SetViewTarget(Character, param);
	}

	ClearPreviousRoom();
	
    PreviousRoomIndex = CurrentRoomIndex;
    CurrentRoomIndex = PendingRoomIndex;
    PendingRoomIndex = INDEX_NONE;
    
    CurrentRoomDynamicInstance = PendingRoomDynamicInstance;
    PendingRoomDynamicInstance = nullptr;

	if(OnTransitionRequestFulfilled.IsBound())
	{
		OnTransitionRequestFulfilled.Broadcast(CurrentRoomIndex);
	}

	RoomTransitionData.Reset();
}
