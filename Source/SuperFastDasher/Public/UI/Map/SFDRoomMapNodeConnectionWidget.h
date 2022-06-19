// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFDRoomMapNodeConnectionWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS(HideDropdown)
class SUPERFASTDASHER_API USFDRoomMapNodeConnectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void AdjustConnectionBetweenPointsAsCanvasPanelSlot(const FVector2D InPointA, const FVector2D InPointB);
	
private:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess))
	UImage* ConnectionImage;
};
