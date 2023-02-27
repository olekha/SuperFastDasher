// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "SFD_BTDecorator_StateCheck.generated.h"

enum class ESFDEnumAIState : uint8;
class AAIController;
class ASFDEnemyPawn;

UCLASS()
class SUPERFASTDASHER_API USFD_BTDecorator_StateCheck : public UBTDecorator
{
	GENERATED_UCLASS_BODY()
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual FString GetStaticDescription() const override;
	
protected:
	
	UPROPERTY(EditAnywhere, Category=Decorator)
	ESFDEnumAIState StateToPass;
};
