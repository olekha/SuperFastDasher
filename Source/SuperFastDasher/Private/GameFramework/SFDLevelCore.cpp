// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/SFDLevelCore.h"

#include "Components/TextRenderComponent.h"
#include "Engine/TextRenderActor.h"
#include "GameFramework/SFDNextRoomLoader.h"
#include "GameFramework/SFDLevelsManager.h"

// Sets default values
ASFDLevelCore::ASFDLevelCore()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ASFDLevelCore::BeginPlay()
{
	Super::BeginPlay();

	ensureAlways(NextRoomLoaders.Num() == 4);
	ensureAlways(RoomsSpawnPoints.Num() == 4);

	for(uint8 i = 0; i < NextRoomLoaders.Num(); ++i)
	{
		ASFDNextRoomLoader* RoomLoader = NextRoomLoaders[i];
		if(!ensureAlways(IsValid(RoomLoader)))
		{
			continue;
		}

		RoomLoader->SetLocalIndex(i);
		RoomLoader->SetActorHiddenInGame(true);
		RoomLoader->SetActorEnableCollision(false);
	}
}

ASFDNextRoomLoader* ASFDLevelCore::GetNextRoomLoaderByRoomIndex(const uint8 InRoomIndex) const
{
	ASFDNextRoomLoader* const * NextRoomLoader = NextRoomLoaders.FindByPredicate([InRoomIndex](const ASFDNextRoomLoader* RoomLoader){ return RoomLoader->GetRoomToLoadIndex() == InRoomIndex;});

	if(NextRoomLoader == nullptr
		|| !IsValid(*NextRoomLoader))
	{
		return nullptr;
	}

	return *NextRoomLoader;
}

ASFDNextRoomLoader* ASFDLevelCore::GetNextRoomLoaderByLocalIndex(const uint8 InLocalIndexIndex) const
{
	if(NextRoomLoaders.IsValidIndex(InLocalIndexIndex))
	{
		return nullptr;
	}

	return NextRoomLoaders[InLocalIndexIndex];
}

const FTransform& ASFDLevelCore::GetNextRoomTransformByRoomIndex(const uint8 InRoomIndex) const
{
	const int32 RoomLoaderIndex = NextRoomLoaders.IndexOfByPredicate([InRoomIndex](const ASFDNextRoomLoader* RoomLoader){ return RoomLoader->GetRoomToLoadIndex() == InRoomIndex;});

	if(RoomLoaderIndex == INDEX_NONE)
	{
		return FTransform::Identity;
	}

	if(!ensureAlways(RoomsSpawnPoints.IsValidIndex(RoomLoaderIndex)))
	{
		return FTransform::Identity;
	}

	return RoomsSpawnPoints[RoomLoaderIndex]->GetActorTransform();
}

const FTransform& ASFDLevelCore::GetNextRoomTransformByLocalIndex(const uint8 InLocalIndexIndex) const
{
	if(!RoomsSpawnPoints.IsValidIndex(InLocalIndexIndex))
 	{
 		return FTransform::Identity;
 	}

	const AActor* RoomSpawnPoint = RoomsSpawnPoints[InLocalIndexIndex];
	if(!ensureAlways(IsValid(RoomSpawnPoint)))
	{
		return FTransform::Identity;
	}

	return RoomSpawnPoint->GetActorTransform();
}

void ASFDLevelCore::SetRoomIndex(const uint8 InRoomIndex)
{
	if(!ensureAlways(InRoomIndex != INDEX_NONE))
	{
		return;
	}
	
	RoomIndex = InRoomIndex;

	if(IsValid(RoomIndexTextRenderActor))
	{
		RoomIndexTextRenderActor->GetTextRender()->SetText(FText::AsNumber(RoomIndex));
	}
	
	UpdateNextRoomsLoaders();
}

void ASFDLevelCore::UpdateNextRoomsLoaders()
{
	if(!ensureAlways(RoomIndex != INDEX_NONE))
	{
		return;
	}

	const USFDLevelsManager* LevelsManager = SFD::GetLevelsManager(this);
	if(!ensureAlways(IsValid(LevelsManager)))
	{
		return;
	}
	
	static TArray<uint8> ConnectedRooms;
	LevelsManager->GetAllConnectionsForRoom(ConnectedRooms, RoomIndex);

	ensureAlways(ConnectedRooms.Num() <= 4);
	
	for(uint8 i = 0; i < ConnectedRooms.Num(); ++i)
	{
		const uint8 ConnectedRoomIndex = ConnectedRooms[i];
		const uint8 LocalIndex = GetFreeLocalIndexForNextRoom(ConnectedRoomIndex);
		
		if(!ensureAlways(NextRoomLoaders.IsValidIndex(LocalIndex)))
		{
			continue;
		}
		
		ASFDNextRoomLoader* RoomLoader = NextRoomLoaders[LocalIndex];
		if(RoomLoader->GetRoomToLoadIndex() == 0xFFu)
		{
			RoomLoader->SetRoomToLoadIndex(ConnectedRoomIndex);
			RoomLoader->SetActorEnableCollision(true);
			RoomLoader->SetActorHiddenInGame(false);
		}
	}
}

uint8 ASFDLevelCore::GetFreeLocalIndexForNextRoom(const uint8 InRoomIndex) const
{
	uint8 LocalIndex = (InRoomIndex / static_cast<uint8>(3));

	if(NextRoomLoaders[LocalIndex]->GetRoomToLoadIndex() != 0xFFu)
	{
		LocalIndex = 0xFFu;
		for(uint8 i = 0; i < NextRoomLoaders.Num(); ++i)
		{
			if(NextRoomLoaders[i]->GetRoomToLoadIndex() == 0xFFu)
			{
				LocalIndex = i;
				break;
			}
		}
	}

	return LocalIndex;
}
