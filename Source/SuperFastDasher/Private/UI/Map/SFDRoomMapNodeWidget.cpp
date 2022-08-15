// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Map/SFDRoomMapNodeWidget.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"

void USFDRoomMapNodeWidget::SetRoomIndex(const uint8 InRoomIndex)
{
	RoomIndex = InRoomIndex;
	
	RoomIndexTextBlock->SetText(FText::AsNumber(InRoomIndex));
}

void USFDRoomMapNodeWidget::SetIsCurrentRoom(const bool bIsCurrentRoom)
{
	OutlineBorder->SetBrushColor(OutlineBorder->BrushColor.CopyWithNewOpacity(bIsCurrentRoom ? 1.0f : 0.0f));

	FSlateFontInfo NewFont = RoomIndexTextBlock->Font;
	NewFont.OutlineSettings.OutlineSize = bIsCurrentRoom ? 2 : 0;
	RoomIndexTextBlock->SetFont(NewFont);

	RoomIndexTextBlock->SetColorAndOpacity(bIsCurrentRoom ? FLinearColor(0.0,0.166104,1.0,1.0) : FLinearColor(1.0,0.806952,0.0, 1.0f)); 
}
