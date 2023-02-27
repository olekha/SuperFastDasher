// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SFD_BTTask_AIBehavior.generated.h"

class ASFDCharacter;
class UBlackboardKeyType_Object;
class AAIController;

UCLASS()
class SUPERFASTDASHER_API USFD_BTTask_AIBehavior : public UBTTaskNode
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector PlayerPwanBBKeySelector;
};
