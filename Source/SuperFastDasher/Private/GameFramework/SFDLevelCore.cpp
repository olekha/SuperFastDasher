// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/SFDLevelCore.h"

#include "Components/TextRenderComponent.h"
#include "Engine/TextRenderActor.h"
#include "GameFramework/SFDNextRoomLoader.h"
#include "GameFramework/SFDLevelsManager.h"
#include "GameFramework/SFDLoadersSpawner.h"
#include "SuperFastDasher/SuperFastDasher.h"

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
	
	ensureAlways(IsValid(RoomLoadersSpawner));
	
	USFDLevelsManager* LevelsManager = SFD::GetLevelsManager(this);
	if(ensureAlways(IsValid(LevelsManager)))
	{
		if(!LevelsManager->GetOnTransitionRequestReceivedDelegate().IsBoundToObject(this))
		{
			LevelsManager->GetOnTransitionRequestReceivedDelegate().AddUObject(this, &ASFDLevelCore::OnTransitionRequestReceived);
		}
		
		if(!LevelsManager->GetOnTransitionRequestFulfilledDelegate().IsBoundToObject(this))
		{
			LevelsManager->GetOnTransitionRequestFulfilledDelegate().AddUObject(this, &ASFDLevelCore::OnTransitionRequestFulfilled);
		}
	}
}

void ASFDLevelCore::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for(uint8 i = 0; i < SpawnedLoaders.Num(); ++i)
    {
        ASFDNextRoomLoader* RoomLoader = SpawnedLoaders[i];
        if(!IsValid(RoomLoader))
        {
        	continue;
        }

        RoomLoader->SetLocalIndex(0xFFu);

        if(RoomLoader->GetOnPlayerStepIntoLoaderDelegate().IsBoundToObject(this))
        {
        	RoomLoader->GetOnPlayerStepIntoLoaderDelegate().RemoveAll(this);
        }

        if(RoomLoader->GetOnPlayerStepOutOfLoaderDelegate().IsBoundToObject(this))
        {
        	RoomLoader->GetOnPlayerStepOutOfLoaderDelegate().RemoveAll(this);
        }
    }

	ClearLoadersActors();

	USFDLevelsManager* LevelsManager = SFD::GetLevelsManager(this);
	if(ensureAlways(IsValid(LevelsManager)))
	{
		if(LevelsManager->GetOnTransitionRequestReceivedDelegate().IsBoundToObject(this))
		{
			LevelsManager->GetOnTransitionRequestReceivedDelegate().RemoveAll(this);
		}
		
		if(LevelsManager->GetOnTransitionRequestFulfilledDelegate().IsBoundToObject(this))
		{
			LevelsManager->GetOnTransitionRequestFulfilledDelegate().RemoveAll(this);
		}
	}
}

void ASFDLevelCore::ClearLoadersActors()
{
	for(uint8 i = 0; i < SpawnedLoaders.Num(); ++i)
	{
		if(IsValid(SpawnedLoaders[i]))
		{
			SpawnedLoaders[i]->Destroy();
		}
	}

	SpawnedLoaders.Empty();
}

ASFDNextRoomLoader* ASFDLevelCore::GetNextRoomLoaderByRoomIndex(const uint8 InRoomIndex) const
{
	ASFDNextRoomLoader* const * NextRoomLoader = SpawnedLoaders.FindByPredicate([InRoomIndex](const ASFDNextRoomLoader* RoomLoader){ return RoomLoader->GetRoomToLoadIndex() == InRoomIndex;});

	if(NextRoomLoader == nullptr
		|| !IsValid(*NextRoomLoader))
	{
		return nullptr;
	}

	return *NextRoomLoader;
}

ASFDNextRoomLoader* ASFDLevelCore::GetNextRoomLoaderByLocalIndex(const uint8 InLocalIndexIndex) const
{
	if(SpawnedLoaders.IsValidIndex(InLocalIndexIndex))
	{
		return nullptr;
	}

	return SpawnedLoaders[InLocalIndexIndex];
}

void ASFDLevelCore::InitializeRoom(const uint8 InRoomIndex)
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
	if(!ensureAlways(RoomIndex != INDEX_NONE)
		|| !ensureAlways(IsValid(RoomLoadersSpawner)))
	{
		return;
	}

	const USFDLevelsManager* LevelsManager = SFD::GetLevelsManager(this);
	if(!ensureAlways(IsValid(LevelsManager)))
	{
		return;
	}
	
	static TArray<uint8> ConnectedRoomsIndices;
	LevelsManager->GetAllConnectionsForRoom(ConnectedRoomsIndices, RoomIndex);

	if (!ensureAlways(ConnectedRoomsIndices.Num() != 0))
	{
		return;
	}

	auto InitializeLoader = [this](ASFDNextRoomLoader*& InLoader, const int32 InLocalIndex, const int32 InLoadRoomIndex, const int32 InOwnerIndex) 
	{
		InLoader->SetLocalIndex(InLocalIndex);
		InLoader->SetRoomToLoadIndex(InLoadRoomIndex);
		InLoader->SetOwnerRoomIndex(InOwnerIndex);

		if (!InLoader->GetOnPlayerStepIntoLoaderDelegate().IsBoundToObject(this))
		{
			InLoader->GetOnPlayerStepIntoLoaderDelegate().AddUObject(this, &ASFDLevelCore::OnPlayerEntersRoomLoader);
		}

		if (!InLoader->GetOnPlayerStepOutOfLoaderDelegate().IsBoundToObject(this))
		{
			InLoader->GetOnPlayerStepOutOfLoaderDelegate().AddUObject(this, &ASFDLevelCore::OnPlayerLeavesRoomLoader);
		}
	};

	const uint8 PreviousRoomIndex = LevelsManager->GetPreviousRoomIndex(RoomIndex);
	
	if(IsValid(PreviousRoomLoaderActor))
	{
		if (PreviousRoomIndex != 0xFFu)
		{
			const bool bRemovedAny = static_cast<bool>(ConnectedRoomsIndices.RemoveSingle(PreviousRoomIndex));

			ensureAlways(bRemovedAny);
		
			InitializeLoader(PreviousRoomLoaderActor, 0, PreviousRoomIndex, RoomIndex);
		}
		else
		{
			PreviousRoomLoaderActor->SetActorEnableCollision(false);
			PreviousRoomLoaderActor->SetActorHiddenInGame(true);
		}
	}
	
	RoomLoadersSpawner->SpawnLoaders(ConnectedRoomsIndices.Num(), SpawnedLoaders);

	ensureAlways(!SpawnedLoaders.IsEmpty() || IsValid(PreviousRoomLoaderActor));

	for(uint8 i = 0; i < SpawnedLoaders.Num(); ++i)
	{
		ASFDNextRoomLoader* RoomLoader = SpawnedLoaders[i];
		if(ensureAlways(IsValid(RoomLoader)))
		{
			InitializeLoader(RoomLoader, i + 1, ConnectedRoomsIndices[i], RoomIndex);
		}	
	}

	if (IsValid(PreviousRoomLoaderActor) && PreviousRoomIndex != INDEX_NONE)
	{
		SpawnedLoaders.Insert(PreviousRoomLoaderActor, 0);
	}
}

void ASFDLevelCore::OnPlayerEntersRoomLoader(uint8 InRoomIndex, uint8 InLoaderLocalIndex)
{
	
}

void ASFDLevelCore::OnPlayerLeavesRoomLoader(uint8 InRoomIndex, uint8 InLoaderLocalIndex)
{
	if(!bIsTeleportsEnabled)
	{
		USFDLevelsManager* LevelsManager = SFD::GetLevelsManager(this);
		if(!ensureAlways(IsValid(LevelsManager)))
		{
			return;
		}
		
		if(RoomIndex == InRoomIndex)
		{
			EnableTeleports();
		}
	}
}

void ASFDLevelCore::OnTransitionRequestReceived(const uint8 InTransitionRoomIndex)
{
	DisableTeleports();
}

void ASFDLevelCore::OnTransitionRequestFulfilled(const uint8 InTransitionRoomIndex)
{
	USFDLevelsManager* LevelsManager = SFD::GetLevelsManager(this);
	if(!ensureAlways(IsValid(LevelsManager)))
	{
		return;
	}

	const FSFDRoomTransitionData& TransitionData = LevelsManager->GetRoomTransitionData();

	if(TransitionData.FromRoom == 0xFFu)
	{
		EnableTeleports();
	}
}
