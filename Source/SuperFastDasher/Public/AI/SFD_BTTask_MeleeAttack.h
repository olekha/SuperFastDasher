// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SFD_BTTask_AIBehavior.h"
#include "SFD_BTTask_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class SUPERFASTDASHER_API USFD_BTTask_MeleeAttack : public USFD_BTTask_AIBehavior
{
	GENERATED_BODY()
	
public:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};