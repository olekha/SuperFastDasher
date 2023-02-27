// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SFD_BTTask_AIBehavior.h"
#include "SFD_BTTask_Distancing.generated.h"

class UAITask_MoveTo;
class UBlackboardComponent;
class UBehaviorTreeComponent;
class UNavigationQueryFilter;
class ASFDEnemyPawn;
class ASFDCharacter;

struct FBTDistancingTaskMemory
{
	/** Move request ID */
	FAIRequestID MoveRequestID;

	FVector PreviousGoalLocation;
	FVector DistancingTargetLocation;
	
	TWeakObjectPtr<UAITask_MoveTo> Task;

	uint8 bObserverCanFinishTask : 1;
};

UCLASS(config=Game, abstract)
class SUPERFASTDASHER_API USFD_BTTask_Distancing : public USFD_BTTask_AIBehavior
{
	GENERATED_UCLASS_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void PostLoad() override;

	virtual void OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 RequestID, bool bSuccess) override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
	virtual void OnNodeCreated() override;
#endif // WITH_EDITOR

protected:

	virtual void PreMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual EBTNodeResult::Type PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
	/** prepares move task for activation */
	virtual UAITask_MoveTo* PrepareMoveTask(UBehaviorTreeComponent& OwnerComp, UAITask_MoveTo* ExistingTask, FAIMoveRequest& MoveRequest);
	
	virtual FVector GetDistancingTargetLocation(const ASFDCharacter* const InPlayerPawn, const ASFDEnemyPawn* const InOwnerPawn) const PURE_VIRTUAL(USFD_BTTask_Distancing::GetDistancingTargetLocation, return FVector::ZeroVector;);

private:

	void TrackTargetLocationAlteration(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory, float InDeltaSeconds);

protected:

	UPROPERTY(Category = Node, EditAnywhere)
	bool bShouldTrackTargetLocationAlteration;
	
private:
	
	/** fixed distance added to threshold between AI and goal location in destination reach test */
	UPROPERTY(Category = Node, EditAnywhere, meta=(ClampMin = "0.0", UIMin="0.0"))
	float AcceptableRadius;
	
	UPROPERTY(Category = Node, EditAnywhere, meta=(ClampMin = "1.0", UIMin="1.0", EditCondition="bShouldTrackTargetLocationAlteration", DisplayAfter="bShouldTrackTargetLocationAlteration"))
	float TargetAlterationTolerance;
	
	/** "None" will result in default filter being used */
	UPROPERTY(Category = Node, EditAnywhere)
	TSubclassOf<UNavigationQueryFilter> FilterClass;

	UPROPERTY(Category = Node, EditAnywhere)
	uint32 bAllowStrafe : 1;

	/** if set, use incomplete path when goal can't be reached */
	UPROPERTY(Category = Node, EditAnywhere, AdvancedDisplay)
	uint32 bAllowPartialPath : 1;

	/** if set, path to goal actor will update itself when actor moves */
	UPROPERTY(Category = Node, EditAnywhere, AdvancedDisplay)
	uint32 bTrackMovingGoal : 1;

	/** if set, goal location will be projected on navigation data (navmesh) before using */
	UPROPERTY(Category = Node, EditAnywhere, AdvancedDisplay)
	uint32 bProjectGoalLocation : 1;

	/** if set, radius of AI's capsule will be added to threshold between AI and goal location in destination reach test  */
	UPROPERTY(Category = Node, EditAnywhere)
	uint32 bReachTestIncludesAgentRadius : 1;
	
	/** if set, radius of goal's capsule will be added to threshold between AI and goal location in destination reach test  */
	UPROPERTY(Category = Node, EditAnywhere)
	uint32 bReachTestIncludesGoalRadius : 1;

	/** DEPRECATED, please use combination of bReachTestIncludes*Radius instead */
	UPROPERTY(Category = Node, VisibleInstanceOnly)
	uint32 bStopOnOverlap : 1;

	UPROPERTY()
	uint32 bStopOnOverlapNeedsUpdate : 1;

	/** if set, move will use pathfinding. Not exposed on purpose, please use BTTask_MoveDirectlyToward */
	uint32 bUsePathfinding : 1;
};
