// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFDNextRoomLoader.generated.h"

class UBoxComponent;
class UTextRenderComponent;

UCLASS()
class SUPERFASTDASHER_API ASFDNextRoomLoader : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	ASFDNextRoomLoader();

	void SetRoomToLoadIndex(const uint8 InRoomIndex);
	
	FORCEINLINE uint8 GetRoomToLoadIndex() const
	{
		return RoomToLoadIndex;
	}
	
	void SetLocalIndex(const uint8 InLocalIndex);
	
	FORCEINLINE uint8 GetLocalIndex() const
	{
		return LocalIndex;
	}
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	
	UFUNCTION()
	void OnBoxComponentOverlapped(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxTrigerComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* NextRoomIndexTextComponent;
	
private:
	
	uint8 RoomToLoadIndex = 0xFFu;

	uint8 LocalIndex = 0xFFu;
};
