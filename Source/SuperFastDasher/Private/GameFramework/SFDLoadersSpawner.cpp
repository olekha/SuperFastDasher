// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/SFDLoadersSpawner.h"

#include "GameFramework/SFDNextRoomLoader.h"

// Sets default values
ASFDLoadersSpawner::ASFDLoadersSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ASFDLoadersSpawner::SpawnLoaders(const int32 InAmount, TArray<ASFDNextRoomLoader*>& OutSpawnedLoadersActors)
{
	if (!OutSpawnedLoadersActors.IsEmpty())
	{
		for (ASFDNextRoomLoader* LoaderPtr : OutSpawnedLoadersActors)
		{
			if (IsValid(LoaderPtr))
			{
				LoaderPtr->Destroy();
			}
		}

		OutSpawnedLoadersActors.Reset();
	}

	UWorld* World = GetWorld();
	if(!IsValid(World)
		|| LoadersTemplateClass == nullptr
		|| InAmount == 0)
	{
		return;
	}

	OutSpawnedLoadersActors.Reserve(InAmount);
	
	const FVector SpawnerLocation = GetActorLocation();
	const FRotator SpawnerRotation = GetActorRotation();

	float LoadersYLocation = SpawnerLocation.Y - (50.0f + SpawnersOffset / 2.0f) * (InAmount - 1);

	const float LoadersXLocation = SpawnerLocation.X;
	const float LoadersZLocation = SpawnerLocation.Z;
	
	OutSpawnedLoadersActors.AddZeroed(InAmount);

	for(int32 i = 0; i < InAmount; ++i)
	{
		FVector LoaderLocation = FVector(LoadersXLocation, LoadersYLocation ,LoadersZLocation);
		
		FVector vec = (LoaderLocation - SpawnerLocation);
		vec = vec.RotateAngleAxis(SpawnerRotation.Yaw, FVector(0.0f, 0.0f, 1.0f));

		LoaderLocation = vec + SpawnerLocation;

		FTransform Transform;
		Transform.SetLocation(LoaderLocation);
		Transform.SetRotation((FRotator(0.0f, (bKeepSpawnersRotationDefault ? DefaultRotation : SpawnerRotation.Yaw), 0.0f).Quaternion()));

		ASFDNextRoomLoader* LoaderActor = World->SpawnActorDeferred<ASFDNextRoomLoader>(LoadersTemplateClass, Transform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		LoaderActor->FinishSpawning(FTransform::Identity, true);

		const int32 RealIndex = bRevertSpawnersOrder ? InAmount - i - 1 : i;

		OutSpawnedLoadersActors[RealIndex] = LoaderActor;

		LoadersYLocation += (100.0f + SpawnersOffset);
	}
}
