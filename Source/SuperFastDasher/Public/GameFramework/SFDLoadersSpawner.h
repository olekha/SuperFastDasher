// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFDLoadersSpawner.generated.h"

class ASFDNextRoomLoader;

USTRUCT(BlueprintType, Blueprintable)
struct FSFDLoaderData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 InLoaderRoomIndex = 0xFFu;
};

UCLASS()
class SUPERFASTDASHER_API ASFDLoadersSpawner : public AActor
{
	GENERATED_BODY()

	friend class ASFDLevelCore;
	
public:	
	// Sets default values for this actor's properties
	ASFDLoadersSpawner();

	UFUNCTION(BlueprintCallable)
	void SpawnLoaders(const int32 InAmount, TArray<ASFDNextRoomLoader*>& OutSpawnedLoadersActors);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TSubclassOf<ASFDNextRoomLoader> LoadersTemplateClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float SpawnersOffset = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bKeepSpawnersRotationDefault = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess, EditCondition = "bKeepSpawnersRotationDefault"))
	float DefaultRotation = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bRevertSpawnersOrder = false;
};
