// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperFastDasher.h"

#include "EngineUtils.h"
#include "Modules/ModuleManager.h"
#include "GameFramework/SFDLevelCore.h"
#include "GameFramework/SFDGameMode.h"
#include "GameFramework/SFDLevelsManager.h"
#include "Player/SFDCharacter.h"
#include "Camera/CameraActor.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, SuperFastDasher, "SuperFastDasher");

FString SFD::AttackStateToString(ESFDAttackState InAttackState)
{
	switch (InAttackState)
	{
	case ESFDAttackState::None:
		return FString("None");
	case ESFDAttackState::Beginning:
		return FString("Beginning");
	case ESFDAttackState::Damage:
		return FString("Damage");
	case ESFDAttackState::Finish:
		return FString("Finish");
	case ESFDAttackState::Max:
		return FString("Max");
	}

	return FString("undefined");
}

ASFDLevelCore* SFD::GetLevelCore(const UObject* InWorldContext, const uint8 InRoomIndex)
{
	if(!ensureAlways(IsValid(InWorldContext)))
	{
		return nullptr;
	}
	
	const UWorld* World = GEngine->GetWorldFromContextObject(InWorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if(!ensureAlways(IsValid(World)))
	{
		return nullptr;
	}
	
	for(TActorIterator<ASFDLevelCore> CoresItr(World); CoresItr; ++CoresItr)
	{
		if(CoresItr->GetRoomIndex() == InRoomIndex)
		{
			return *CoresItr;
		}
	}

	return nullptr;
}

ASFDLevelCore* SFD::GetLevelCore(const ULevel* InLevel)
{
	if(!ensureAlways(IsValid(InLevel)))
	{
		return nullptr;
	}
	
	const UWorld* World = GEngine->GetWorldFromContextObject(InLevel, EGetWorldErrorMode::LogAndReturnNull);
	if(!ensureAlways(IsValid(World)))
	{
		return nullptr;
	}
	
	ASFDLevelCore* CoreToReturn = nullptr;
	
	for(TActorIterator<ASFDLevelCore> It(World); It; ++It)
	{
		//ensureAlwaysMsgf(!bIsCoreFound, TEXT("More than one LevelCore are presented in the world somehow. There should be only one LevelCore at time"));
		ASFDLevelCore* Actor = *It;
		check(Actor != nullptr);
		if (IsValid(Actor))
		{
			CoreToReturn = Actor;
		}
	}

	return CoreToReturn;
}

ASFDGameMode* SFD::GetGameMode(const UObject* InWorldContext)
{
	if(!IsValid(InWorldContext))
	{
		return nullptr;
	}

	const UWorld* World = GEngine->GetWorldFromContextObject(InWorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if(!ensureAlways(IsValid(World)))
	{
		return nullptr;
	}

	return World->GetAuthGameMode<ASFDGameMode>();
}

ASFDCharacter* SFD::GetCharacter(const UObject* InWorldContext)
{
	if(!IsValid(InWorldContext))
	{
		return nullptr;
	}
	const UWorld* World = GEngine->GetWorldFromContextObject(InWorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if(!ensureAlways(IsValid(World)))
	{
		return nullptr;
	}
	
	const APlayerController* PlayerController = World->GetFirstPlayerController();
	if(!ensureAlways(IsValid(PlayerController)))
	{
		return nullptr;
	}
	
	return Cast<ASFDCharacter>(PlayerController->GetPawn());
}

APlayerController* SFD::GetPlayerController(const UObject* InWorldContext)
{
	if(!IsValid(InWorldContext))
	{
		return nullptr;
	}
	const UWorld* World = GEngine->GetWorldFromContextObject(InWorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if(!ensureAlways(IsValid(World)))
	{
		return nullptr;
	}
	
	return World->GetFirstPlayerController();
}

USFDLevelsManager* SFD::GetLevelsManager(const UObject* InWorldContext)
{
	const ASFDGameMode* GameMode = SFD::GetGameMode(InWorldContext);
	if(!ensureAlways(IsValid(GameMode)))
    {
    	return nullptr;
    }

	return GameMode->GetLevelsManager();
}

ACameraActor* SFD::GetCameraActorForTransitionBetweenRooms(const UObject* InWorldContext)
{
	if(!IsValid(InWorldContext))
	{
		return nullptr;
	}
	
	const UWorld* World = GEngine->GetWorldFromContextObject(InWorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if(!ensureAlways(IsValid(World)))
	{
		return nullptr;
	}

	const TActorIterator<ACameraActor> CameraActorItr(World);

	return static_cast<bool>(CameraActorItr) ? *CameraActorItr : nullptr;
}
