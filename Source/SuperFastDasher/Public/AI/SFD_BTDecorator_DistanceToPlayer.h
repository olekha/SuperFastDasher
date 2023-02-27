// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "SFD_BTDecorator_DistanceToPlayer.generated.h"

class UBehaviorTreeComponent;

UENUM(BlueprintType)
enum class ESFDComparisonRule : uint8
{
	None				UMETA(Hidden),
	LessOrEqual,
	Equal,
	GreaterOrEqual,
	Max					UMETA(Hidden)
};


UCLASS()
class SUPERFASTDASHER_API USFD_BTDecorator_DistanceToPlayer : public UBTDecorator
{
	GENERATED_UCLASS_BODY()

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual FString GetStaticDescription() const override;

protected:

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector PlayerPwanBBKeySelector;
	
	UPROPERTY(EditAnywhere, Category=Decorator)
	ESFDComparisonRule ComparisonRuleToUse;

	UPROPERTY(EditAnywhere, Category=Decorator,  meta=(ClampMin = "0.0", UIMin="0.0"))
	float ThresholdDistance;
	
};
