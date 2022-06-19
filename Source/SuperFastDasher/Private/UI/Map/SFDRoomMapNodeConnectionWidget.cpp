// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Map/SFDRoomMapNodeConnectionWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void USFDRoomMapNodeConnectionWidget::AdjustConnectionBetweenPointsAsCanvasPanelSlot(const FVector2D InPointA, const FVector2D InPointB)
{
	if(!IsValid(ConnectionImage))
	{
		return;
	}

	UCanvasPanelSlot* AsCanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
	if(!ensureAlways(IsValid(AsCanvasPanelSlot)))
	{
		return;
	}
	
	AsCanvasPanelSlot->SetAutoSize(true);
	AsCanvasPanelSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
	AsCanvasPanelSlot->SetAlignment(FVector2D(0.5f, 1.0f));
	
	AsCanvasPanelSlot->SetPosition(InPointA);
	AsCanvasPanelSlot->SetZOrder(0);
	
	ConnectionImage->SetRenderTransformPivot(FVector2D(0.5f, 1.0f));

	const FVector2D Vector = InPointB - InPointA;
	
	const FSlateBrush& Brush = ConnectionImage->Brush;
	ConnectionImage->SetDesiredSizeOverride(FVector2D(Brush.ImageSize.X, Vector.Length()));
	
	const float Angle = FMath::RadiansToDegrees(FMath::Atan2(Vector.Y, Vector.X));
	ConnectionImage->SetRenderTransformAngle(90.0f + Angle);
}
