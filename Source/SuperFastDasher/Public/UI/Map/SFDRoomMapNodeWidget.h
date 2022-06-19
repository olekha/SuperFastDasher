// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFDRoomMapNodeWidget.generated.h"

class UTextBlock;
class UBorder;

UCLASS(HideDropdown)
class SUPERFASTDASHER_API USFDRoomMapNodeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	void SetRoomIndex(const uint8 InRoomIndex);

	void SetIsCurrentRoom(const bool bIsCurrentRoom);
	
private:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess))
	UBorder* NodeBackgroundBorder;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess))
	UBorder* OutlineBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess))
	UTextBlock* RoomIndexTextBlock;
	
};
