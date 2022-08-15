// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/TransformCalculus3D.h"
#include "SFDNextRoomLoader.generated.h"

class UBoxComponent;
class UTextRenderComponent;

DECLARE_EVENT_TwoParams(ASFDNextRoomLoader, FRoomLoaderDelegatem, uint8 /*InRoomIndex*/, uint8/*InLoaderLocalIndex*/);

UCLASS(HideDropdown)
class SUPERFASTDASHER_API ASFDNextRoomLoader : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	ASFDNextRoomLoader();

	void SetRoomToLoadIndex(const uint8 InRoomIndex);
	FORCEINLINE uint8 GetRoomToLoadIndex() const;
	
	void SetLocalIndex(const uint8 InLocalIndex);
	FORCEINLINE uint8 GetLocalIndex() const;
	
	void SetOwnerRoomIndex(const uint8 InNewOwnerRoomIndex);
	FORCEINLINE uint8 GetOwnerRoomIndex() const;
	
	FORCEINLINE FTransform GetTransformToSpawnPlayer() const;
	
	//(RoomIndex, LoaderLocalIndex)
	FORCEINLINE FRoomLoaderDelegatem& GetOnPlayerStepIntoLoaderDelegate();
	//(RoomIndex, LoaderLocalIndex)
	FORCEINLINE FRoomLoaderDelegatem& GetOnPlayerStepOutOfLoaderDelegate();
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	
	UFUNCTION()
	void OnBoxComponentBeginOverlapped(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );
		
	UFUNCTION()
	void OnBoxComponentEndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxTrigerComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextRenderComponent* NextRoomIndexTextComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* PlayerSpawnPointComponent;
	
private:

	//(RoomIndex, LoaderLocalIndex)
	FRoomLoaderDelegatem OnPlayerStepIntoLoader;
	//(RoomIndex, LoaderLocalIndex)
	FRoomLoaderDelegatem OnPlayerStepOutOfLoader;
	
	uint8 RoomToLoadIndex = 0xFFu;
	uint8 OwnerRoomIndex = 0xFFu;
	uint8 LocalIndex = 0xFFu;
};

FORCEINLINE uint8 ASFDNextRoomLoader::GetRoomToLoadIndex() const
{
	return RoomToLoadIndex;
}

FORCEINLINE uint8 ASFDNextRoomLoader::GetLocalIndex() const
{
	return LocalIndex;
}

FORCEINLINE uint8 ASFDNextRoomLoader::GetOwnerRoomIndex() const
{
	return OwnerRoomIndex;
}

FORCEINLINE FTransform ASFDNextRoomLoader::GetTransformToSpawnPlayer() const
{
	return FTransform(GetActorQuat(), PlayerSpawnPointComponent->GetComponentLocation(), FVector::OneVector);;
}

FORCEINLINE FRoomLoaderDelegatem& ASFDNextRoomLoader::GetOnPlayerStepIntoLoaderDelegate()
{
	return OnPlayerStepIntoLoader;
}

FORCEINLINE FRoomLoaderDelegatem& ASFDNextRoomLoader::GetOnPlayerStepOutOfLoaderDelegate()
{
	return OnPlayerStepOutOfLoader;
}