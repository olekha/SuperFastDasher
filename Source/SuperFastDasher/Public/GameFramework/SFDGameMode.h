// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SFDGameMode.generated.h"

class USFDLevelsManager;
/**
 * 
 */
UCLASS()
class SUPERFASTDASHER_API ASFDGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	FORCEINLINE USFDLevelsManager* GetLevelsManager() const
	{
		return LevelsManager;
	}
		
protected:

	virtual void BeginPlay() override;
	
private:
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess))
	TSubclassOf<USFDLevelsManager> LevelsManagerClass;
	
	UPROPERTY(Transient)
	USFDLevelsManager* LevelsManager;
};
