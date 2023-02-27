// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/SFD_BTTask_Distancing.h"
#include "SFD_BTTask_IncreaseDistance.generated.h"

/**
 * 
 */
UCLASS()
class SUPERFASTDASHER_API USFD_BTTask_IncreaseDistance : public USFD_BTTask_Distancing
{
	GENERATED_UCLASS_BODY()

public:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	virtual FString GetStaticDescription() const override;
	
	virtual FVector GetDistancingTargetLocation(const ASFDCharacter* const InPlayerPawn, const ASFDEnemyPawn* const InOwnerPawn) const override;
	
	virtual void PreMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	void KeepLookAtPlayerRotation(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory, float InDeltaSeconds);
	
protected:
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin="0.0"), Category = Node)
	float DistanceToCover;

	UPROPERTY(EditAnywhere, Category = Node)
	bool bShouldFacePlayer;
};
