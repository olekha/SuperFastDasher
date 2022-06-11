// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperFastDasher.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, SuperFastDasher, "SuperFastDasher");

FString AttackStateToString(ESFDAttackState InAttackState)
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
