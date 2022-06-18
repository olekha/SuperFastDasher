// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/SFDGameMode.h"

#include "GameFramework/SFDLevelsManager.h"

void ASFDGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if(ensureMsgf(LevelsManagerClass != nullptr, TEXT("ASFDGameMode::BeginPlay: The level manager class doesn't set set")))
	{
		LevelsManager = NewObject<USFDLevelsManager>(this, LevelsManagerClass);
		if(ensureMsgf(LevelsManager != nullptr, TEXT("ASFDGameMode::BeginPlay: Couldn't create Levels Manager")))
		{
			LevelsManager->SpawnStartRoom();
		}
	}
}
