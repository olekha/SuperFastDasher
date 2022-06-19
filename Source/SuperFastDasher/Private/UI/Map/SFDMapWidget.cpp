// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Map/SFDMapWidget.h"

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

void USFDMapWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

#if WITH_EDITORONLY_DATA
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
	
	const float RoomsOffsetAngle = 360.0f / static_cast<float>(InRoomsAmount);

	float OffsetsAnglesSum = 0.0f;
	
	for(uint8 i = 0; i < InRoomsAmount; ++i)
	{
		USFDRoomMapNodeWidget* RoomNodeWidget = CreateWidget<USFDRoomMapNodeWidget>(this, RoomWidgetTemplate);
		RoomNodeWidget->SetRoomIndex(i);
		
		FVector2D Position = FVector2D(static_cast<float>(RoomsArrangementCircleRadius) * FMath::Sin(FMath::DegreesToRadians(-OffsetsAnglesSum)),
			static_cast<float>(RoomsArrangementCircleRadius) * FMath::Cos(FMath::DegreesToRadians(-OffsetsAnglesSum)));

		const FVector2D Direction = Position / Position.Length();

		Position += Direction * (i % 2 ? OddRoomsNodesOffset : EvenRoomsNodesOffset);
		
		UCanvasPanelSlot* CanvasPanelSlot = MapCanvasPanel->AddChildToCanvas(RoomNodeWidget);
		CanvasPanelSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		CanvasPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		CanvasPanelSlot->SetAutoSize(true);
		CanvasPanelSlot->SetPosition(Position);
		CanvasPanelSlot->SetZOrder(10);
		
		OffsetsAnglesSum += RoomsOffsetAngle;
	}

	if(InCurrentRoom != INDEX_NONE)
	{
		USFDRoomMapNodeWidget* InCurrentRoomNodeWidget = Cast<USFDRoomMapNodeWidget>(MapCanvasPanel->GetChildAt(InCurrentRoom));

		InCurrentRoomNodeWidget->SetIsCurrentRoom(true);
	}
	
	const TArray<UPanelSlot*>& RoomsSlots = MapCanvasPanel->GetSlots();
	
	for(uint8 i = 0; i < InRoomsAmount; ++i)
	{
		static TArray<uint8> Connections;
		USFDLevelsManager::GetAllConnectionsForRoom(Connections, i, InIncidenceMatrix, InRoomsAmount - 1, InRoomsAmount);

		for(uint8 j = 0; j < Connections.Num(); ++j)
		{
			const uint8 ConnectedRoom = Connections[j];
			if(ConnectedRoom < i)
			{
				continue;
			}
			
			if(!ensureAlways(RoomsSlots.IsValidIndex(i))
				|| !ensureAlways(RoomsSlots.IsValidIndex(j))
				|| !IsValid(RoomsSlots[i])
				|| !IsValid(RoomsSlots[j]))
			{
				continue;
			}

			const UCanvasPanelSlot* AsCanvasPanelSlotA = CastChecked<UCanvasPanelSlot>(RoomsSlots[i]);
			const UCanvasPanelSlot* AsCanvasPanelSlotB = CastChecked<UCanvasPanelSlot>(RoomsSlots[ConnectedRoom]);
			
			USFDRoomMapNodeConnectionWidget* ConnectionWidget = CreateWidget<USFDRoomMapNodeConnectionWidget>(this, RoomConnectionWidgetTemplate);
			MapCanvasPanel->AddChild(ConnectionWidget);
			
			ConnectionWidget->AdjustConnectionBetweenPointsAsCanvasPanelSlot(AsCanvasPanelSlotA->GetPosition(), AsCanvasPanelSlotB->GetPosition());
		}
	}
}
