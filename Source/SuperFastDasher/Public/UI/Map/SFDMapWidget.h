// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFDMapWidget.generated.h"

class USFDRoomMapNodeConnectionWidget;
class UCanvasPanel;
class USFDRoomMapNodeWidget;

UCLASS(HideDropdown)
class SUPERFASTDASHER_API USFDMapWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	virtual void NativePreConstruct() override;
	
private:

	void BuildMap(const uint8 InRoomsAmount, const uint8* const * const InIncidenceMatrix, int8 InCurrentRoom = INDEX_NONE);
	
private:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TSubclassOf<USFDRoomMapNodeWidget> RoomWidgetTemplate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TSubclassOf<USFDRoomMapNodeConnectionWidget> RoomConnectionWidgetTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 RoomsArrangementCircleRadius = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	FVector2D EvenRoomsNodesOffset = FVector2D(0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	FVector2D OddRoomsNodesOffset = FVector2D(0.0f, 0.0f);

#if WITH_EDITORONLY_DATA
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int32 RoomsAmount_DEBUG = 12;

#endif
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess))
	UCanvasPanel* MapCanvasPanel;
	
};
