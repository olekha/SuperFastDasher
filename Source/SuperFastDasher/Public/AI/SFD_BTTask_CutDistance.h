// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SFD_BTTask_Distancing.h"
#include "SFD_BTTask_CutDistance.generated.h"

/**
 * 
 */
UCLASS()
class SUPERFASTDASHER_API USFD_BTTask_CutDistance : public USFD_BTTask_Distancing
{
	GENERATED_UCLASS_BODY()
	
public:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FVector GetDistancingTargetLocation(const ASFDCharacter* const InPlayerPawn, const ASFDEnemyPawn* const InOwnerPawn) const override;

	virtual FString GetStaticDescription() const override;

	virtual void PreMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
