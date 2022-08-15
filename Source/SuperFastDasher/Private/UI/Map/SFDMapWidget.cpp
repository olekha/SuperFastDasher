// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Map/SFDMapWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "GameFramework/SFDLevelsManager.h"
#include "SuperFastDasher/SuperFastDasher.h"
#include "UI/Map/SFDRoomMapNodeConnectionWidget.h"
#include "UI/Map/SFDRoomMapNodeWidget.h"

void USFDMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const USFDLevelsManager* LevelsManager = SFD::GetLevelsManager(this);
	if(ensureAlways(IsValid(LevelsManager)))
	{
		BuildMap(LevelsManager->GetRoomsAmount(), LevelsManager->GetIncidenceMatrix(), LevelsManager->GetCurrentRoomIndex());
	}
}

void USFDMapWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void USFDMapWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

#if WITH_EDITORONLY_DATA && WITH_EDITOR
	if(IsDesignTime())
	{
		uint8** DebugAdjacencyMatrix = nullptr;
		USFDLevelsManager::InitAdjacency(DebugAdjacencyMatrix, RoomsAmount_DEBUG);
		
		uint8** DebugIncidenceMatrix = nullptr;
		USFDLevelsManager::FindMST(DebugIncidenceMatrix, DebugAdjacencyMatrix, RoomsAmount_DEBUG);
	
		BuildMap(RoomsAmount_DEBUG, DebugIncidenceMatrix, 0);
		
		USFDLevelsManager::ClearMatrix(DebugAdjacencyMatrix, RoomsAmount_DEBUG);
		USFDLevelsManager::ClearMatrix(DebugIncidenceMatrix, RoomsAmount_DEBUG);
	}
#endif
}

void USFDMapWidget::BuildMap(const uint8 InRoomsAmount, const uint8* const * const InIncidenceMatrix, int8 InCurrentRoom /*= INDEX_NONE*/)
{
	if(!ensureAlways(RoomWidgetTemplate != nullptr)
		|| !ensureAlways(RoomConnectionWidgetTemplate != nullptr)
		|| !IsValid(MapCanvasPanel))
	{
		return;
	}
	
	MapCanvasPanel->ClearChildren();
	RoomsNodesWidgets.Reset();

	FVector2D GraphStartPoint = FVector2D(0.0f, 0.0f);
	CreateRoomWidget(0,0xFFu,InRoomsAmount, InIncidenceMatrix, GraphStartPoint);
		
	Algo::Sort(RoomsNodesWidgets, [](const USFDRoomMapNodeWidget* const A, const USFDRoomMapNodeWidget* const B){ return A->GetRoomIndex() < B->GetRoomIndex(); });
	
	if(RoomsNodesWidgets.IsValidIndex(InCurrentRoom))
	{
		RoomsNodesWidgets[InCurrentRoom]->SetIsCurrentRoom(true);
	}

	for(int i = 0; i < RoomsNodesWidgets.Num(); ++i)
	{
		const FVector2D RoomAPosition = Cast<UCanvasPanelSlot>(RoomsNodesWidgets[i]->Slot)->GetPosition(); 
		
		TArray<uint8> Connections;
		USFDLevelsManager::GetAllConnectionsForRoom(Connections, i, InIncidenceMatrix, InRoomsAmount - 1, InRoomsAmount);
		
		for(uint8 j = 0; j < Connections.Num(); ++j)
		{
			const uint8 ConnectedRoom = Connections[j];
			if(ConnectedRoom < i)
			{
				continue;
			}
			
			const FVector2D RoomBPosition = Cast<UCanvasPanelSlot>(RoomsNodesWidgets[ConnectedRoom]->Slot)->GetPosition();
			
			USFDRoomMapNodeConnectionWidget* ConnectionWidget = CreateWidget<USFDRoomMapNodeConnectionWidget>(this, RoomConnectionWidgetTemplate);
			MapCanvasPanel->AddChild(ConnectionWidget);

			ConnectionWidget->AdjustConnectionBetweenPointsAsCanvasPanelSlot(RoomAPosition + 35.0f, RoomBPosition + 35.0f);
		}
	}
}

void USFDMapWidget::CreateRoomWidget(const uint8 InRoomIndex,
	const uint8 InWentFromRoomIndex,
	const uint8 InRoomsAmount,
	const uint8* const * const InIncidenceMatrix,
	FVector2D& InNodeLocation)
{
	TArray<uint8> Connections;
	USFDLevelsManager::GetAllConnectionsForRoom(Connections, InRoomIndex, InIncidenceMatrix, InRoomsAmount - 1, InRoomsAmount);

	Connections.RemoveSingle(InWentFromRoomIndex);
	
	USFDRoomMapNodeWidget* RoomNodeWidget = CreateWidget<USFDRoomMapNodeWidget>(this, RoomWidgetTemplate);
	RoomNodeWidget->SetRoomIndex(InRoomIndex);
	
	RoomsNodesWidgets.Add(RoomNodeWidget);

	UCanvasPanelSlot* RoomNodeWidget_CPSlot = MapCanvasPanel->AddChildToCanvas(RoomNodeWidget);
	RoomNodeWidget_CPSlot->SetAutoSize(true);
	RoomNodeWidget_CPSlot->SetAlignment(FVector2D(0.0f, 0.0f));
	RoomNodeWidget_CPSlot->SetZOrder(1);
	
	FVector2D ActualNodeLocation = InNodeLocation;
	
	if(Connections.Num() != 0)
	{
		const FVector2D NeighboursStartLocation = FVector2D(InNodeLocation.X + 70.0f + RoomsNodesOffset.X, InNodeLocation.Y);
	
		FVector2D NeighbourLocation = NeighboursStartLocation;
		
		for(uint8 i = 0; i < Connections.Num(); ++i)
		{
			CreateRoomWidget(Connections[i], InRoomIndex, InRoomsAmount, InIncidenceMatrix, NeighbourLocation);

			if(i != Connections.Num() - 1)
			{
				NeighbourLocation.Y += RoomsNodesOffset.Y + 70.0f;
			}
		}
		
		ActualNodeLocation.Y = NeighbourLocation.Y - (NeighbourLocation.Y  - NeighboursStartLocation.Y) / 2.0f;
		InNodeLocation.Y = NeighbourLocation.Y;
	}
	
	RoomNodeWidget_CPSlot->SetPosition(ActualNodeLocation);
}
