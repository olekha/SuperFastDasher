// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SFD_BTTask_Distancing.h"
#include "GameFramework/Actor.h"
#include "AISystem.h"
#include "AIController.h"
#include "AI/SFDEnemyPawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Tasks/AITask_MoveTo.h"
#include "VisualLogger/VisualLogger.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavFilters/NavigationQueryFilter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/SFDCharacter.h"

USFD_BTTask_Distancing::USFD_BTTask_Distancing(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "SFD Distancing";
	INIT_TASK_NODE_NOTIFY_FLAGS();

	AcceptableRadius = GET_AI_CONFIG_VAR(AcceptanceRadius);
	bReachTestIncludesGoalRadius = bReachTestIncludesAgentRadius = bStopOnOverlap = GET_AI_CONFIG_VAR(bFinishMoveOnGoalOverlap);
	bAllowStrafe = GET_AI_CONFIG_VAR(bAllowStrafing);
	bAllowPartialPath = GET_AI_CONFIG_VAR(bAcceptPartialPaths);
	bTrackMovingGoal = true;
	bProjectGoalLocation = true;
	bUsePathfinding = true;

	bStopOnOverlapNeedsUpdate = true;

	AcceptableRadius = 100.0f;
	TargetAlterationTolerance = 50.0f;
	bShouldTrackTargetLocationAlteration = true;
}

EBTNodeResult::Type USFD_BTTask_Distancing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const AAIController* const OwnerController = Cast<AAIController>(OwnerComp.GetOwner());
	const ASFDEnemyPawn* const OwnerPawn = Cast<ASFDEnemyPawn>(OwnerController->GetPawn());

	const UBlackboardComponent* const BlackboardComp = OwnerComp.GetBlackboardComponent();
	const ASFDCharacter* const PlayerPawn = Cast<ASFDCharacter>(BlackboardComp->GetValueAsObject(PlayerPwanBBKeySelector.SelectedKeyName));
	
	if(!IsValid(PlayerPawn)
		|| !IsValid(OwnerPawn))
	{
		return EBTNodeResult::Failed;
	}
	
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	FBTDistancingTaskMemory* MyMemory = CastInstanceNodeMemory<FBTDistancingTaskMemory>(NodeMemory);
	MyMemory->PreviousGoalLocation = FAISystem::InvalidLocation;
	MyMemory->MoveRequestID = FAIRequestID::InvalidRequest;
	
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (MyController == nullptr)
	{
		UE_VLOG(OwnerComp.GetOwner(), LogBehaviorTree, Error, TEXT("USFD_BTTask_Distancing::ExecuteTask failed since AIController is missing."));
		NodeResult = EBTNodeResult::Failed;
	}
	else
	{
		MyMemory->DistancingTargetLocation = GetDistancingTargetLocation(PlayerPawn, OwnerPawn);

		PreMoveTask(OwnerComp, NodeMemory);
		
		NodeResult = PerformMoveTask(OwnerComp, NodeMemory);
	}
	
	return NodeResult;
}

EBTNodeResult::Type USFD_BTTask_Distancing::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTDistancingTaskMemory* MyMemory = CastInstanceNodeMemory<FBTDistancingTaskMemory>(NodeMemory);
	if (MyMemory->MoveRequestID.IsValid())
	{
		AAIController* MyController = OwnerComp.GetAIOwner();
		if (MyController && MyController->GetPathFollowingComponent())
		{
			MyController->GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished, MyMemory->MoveRequestID);
		}
	}
	else
	{
		MyMemory->bObserverCanFinishTask = false;
		UAITask_MoveTo* MoveTask = MyMemory->Task.Get();
		if (MoveTask)
		{
			MoveTask->ExternalCancel();
		}
	}

	return Super::AbortTask(OwnerComp, NodeMemory);
}

void USFD_BTTask_Distancing::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	FBTDistancingTaskMemory* MyMemory = CastInstanceNodeMemory<FBTDistancingTaskMemory>(NodeMemory);
	MyMemory->Task.Reset();
	
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void USFD_BTTask_Distancing::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if(bShouldTrackTargetLocationAlteration)
	{
		TrackTargetLocationAlteration(OwnerComp, NodeMemory, DeltaSeconds);
	}
}

uint16 USFD_BTTask_Distancing::GetInstanceMemorySize() const
{
	return sizeof(FBTDistancingTaskMemory);
}

void USFD_BTTask_Distancing::PostLoad()
{
	Super::PostLoad();

	if (bStopOnOverlapNeedsUpdate)
	{
		bStopOnOverlapNeedsUpdate = false;
		bReachTestIncludesAgentRadius = bStopOnOverlap;
		bReachTestIncludesGoalRadius = false;
	}
}

void USFD_BTTask_Distancing::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	// AI move task finished
	UAITask_MoveTo* MoveTask = Cast<UAITask_MoveTo>(&Task);
	if (MoveTask && MoveTask->GetAIController() && MoveTask->GetState() != EGameplayTaskState::Paused)
	{
		UBehaviorTreeComponent* BehaviorComp = GetBTComponentForTask(Task);
		if (BehaviorComp)
		{
			uint8* RawMemory = BehaviorComp->GetNodeMemory(this, BehaviorComp->FindInstanceContainingNode(this));
			const FBTDistancingTaskMemory* MyMemory = CastInstanceNodeMemory<FBTDistancingTaskMemory>(RawMemory);

			if (MyMemory && MyMemory->bObserverCanFinishTask && (MoveTask == MyMemory->Task))
			{
				const bool bSuccess = MoveTask->WasMoveSuccessful();
				FinishLatentTask(*BehaviorComp, bSuccess ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
			}
		}
	}
}

void USFD_BTTask_Distancing::OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 RequestID, bool bSuccess)
{
	// AIMessage_RepathFailed means task has failed
	bSuccess &= (Message != UBrainComponent::AIMessage_RepathFailed);
	Super::OnMessage(OwnerComp, NodeMemory, Message, RequestID, bSuccess);
}

#if WITH_EDITOR
FName USFD_BTTask_Distancing::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Task.MoveTo.Icon");
}

void USFD_BTTask_Distancing::OnNodeCreated()
{
	bStopOnOverlapNeedsUpdate = false;
}

#endif // WITH_EDITOR

void USFD_BTTask_Distancing::PreMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

}

EBTNodeResult::Type USFD_BTTask_Distancing::PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	FBTDistancingTaskMemory* MyMemory = CastInstanceNodeMemory<FBTDistancingTaskMemory>(NodeMemory);
	AAIController* MyController = OwnerComp.GetAIOwner();

	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;
	if (MyController && MyBlackboard)
	{
		FAIMoveRequest MoveReq;
		MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : MyController->GetDefaultNavigationFilterClass());
		MoveReq.SetAllowPartialPath(bAllowPartialPath);
		MoveReq.SetAcceptanceRadius(AcceptableRadius);
		MoveReq.SetCanStrafe(bAllowStrafe);
		MoveReq.SetReachTestIncludesAgentRadius(bReachTestIncludesAgentRadius);
		MoveReq.SetReachTestIncludesGoalRadius(bReachTestIncludesGoalRadius);
		MoveReq.SetProjectGoalLocation(bProjectGoalLocation);
		MoveReq.SetUsePathfinding(bUsePathfinding);
		
		const FVector TargetLocation = MyMemory->DistancingTargetLocation;
		MoveReq.SetGoalLocation(TargetLocation);

		UKismetSystemLibrary::DrawDebugSphere(this, TargetLocation, 30.0f, 20.0f, FLinearColor::Green, 2.0f, 0.0f);
		
		MyMemory->PreviousGoalLocation = TargetLocation;

		if (MoveReq.IsValid())
		{
			if (GET_AI_CONFIG_VAR(bEnableBTAITasks))
			{
				UAITask_MoveTo* MoveTask = MyMemory->Task.Get();
				const bool bReuseExistingTask = (MoveTask != nullptr);

				MoveTask = PrepareMoveTask(OwnerComp, MoveTask, MoveReq);
				if (MoveTask)
				{
					MyMemory->bObserverCanFinishTask = false;

					if (bReuseExistingTask)
					{
						if (MoveTask->IsActive())
						{
							UE_VLOG(MyController, LogBehaviorTree, Verbose, TEXT("\'%s\' reusing AITask %s"), *GetNodeName(), *MoveTask->GetName());
							MoveTask->ConditionalPerformMove();
						}
						else
						{
							UE_VLOG(MyController, LogBehaviorTree, Verbose, TEXT("\'%s\' reusing AITask %s, but task is not active - handing over move performing to task mechanics"), *GetNodeName(), *MoveTask->GetName());
						}
					}
					else
					{
						MyMemory->Task = MoveTask;
						UE_VLOG(MyController, LogBehaviorTree, Verbose, TEXT("\'%s\' task implementing move with task %s"), *GetNodeName(), *MoveTask->GetName());
						MoveTask->ReadyForActivation();
					}

					MyMemory->bObserverCanFinishTask = true;
					NodeResult = (MoveTask->GetState() != EGameplayTaskState::Finished) ? EBTNodeResult::InProgress :
						MoveTask->WasMoveSuccessful() ? EBTNodeResult::Succeeded :
						EBTNodeResult::Failed;
				}
			}
			else
			{
				const FPathFollowingRequestResult RequestResult = MyController->MoveTo(MoveReq);
				if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful)
				{
					MyMemory->MoveRequestID = RequestResult.MoveId;
					WaitForMessage(OwnerComp, UBrainComponent::AIMessage_MoveFinished, RequestResult.MoveId);
					WaitForMessage(OwnerComp, UBrainComponent::AIMessage_RepathFailed);

					NodeResult = EBTNodeResult::InProgress;
				}
				else if (RequestResult.Code == EPathFollowingRequestResult::AlreadyAtGoal)
				{
					NodeResult = EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return NodeResult;
}

UAITask_MoveTo* USFD_BTTask_Distancing::PrepareMoveTask(UBehaviorTreeComponent& OwnerComp, UAITask_MoveTo* ExistingTask, FAIMoveRequest& MoveRequest)
{
	UAITask_MoveTo* MoveTask = ExistingTask ? ExistingTask : NewBTAITask<UAITask_MoveTo>(OwnerComp);
	if (MoveTask)
	{
		MoveTask->SetUp(MoveTask->GetAIController(), MoveRequest);
	}

	return MoveTask;
}

void USFD_BTTask_Distancing::TrackTargetLocationAlteration(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory, float InDeltaSeconds)
{
	const AAIController* const OwnerController = Cast<AAIController>(InOwnerComp.GetOwner());
	const ASFDEnemyPawn* const OwnerPawn = Cast<ASFDEnemyPawn>(OwnerController->GetPawn());

	const UBlackboardComponent* const BlackboardComp = InOwnerComp.GetBlackboardComponent();
	const ASFDCharacter* const PlayerPawn = Cast<ASFDCharacter>(BlackboardComp->GetValueAsObject(PlayerPwanBBKeySelector.SelectedKeyName));
	
	if(!IsValid(PlayerPawn)
		|| !IsValid(OwnerPawn))
	{
		return;
	}
	
	uint8* RawMemory = InOwnerComp.GetNodeMemory(this, InOwnerComp.FindInstanceContainingNode(this));
	FBTDistancingTaskMemory* MyMemory = CastInstanceNodeMemory<FBTDistancingTaskMemory>(RawMemory);

	const EBTTaskStatus::Type TaskStatus = InOwnerComp.GetTaskStatus(this);
	if (TaskStatus != EBTTaskStatus::Active
		|| MyMemory == nullptr
		|| InOwnerComp.GetAIOwner() == nullptr)
	{
		return;
	}
	
	check(InOwnerComp.GetAIOwner()->GetPathFollowingComponent());

	const FVector NewTargetLocation = GetDistancingTargetLocation(PlayerPawn, OwnerPawn);
	
	const bool bUpdateMove = (NewTargetLocation - MyMemory->DistancingTargetLocation).Length() > TargetAlterationTolerance;

	if (!bUpdateMove)
	{
		return;
	}
	
	// don't abort move if using AI tasks - it will mess things up
	if (MyMemory->MoveRequestID.IsValid())
	{
		UE_VLOG(OwnerController, LogBehaviorTree, Log, TEXT("Blackboard value for goal has changed, aborting current move request"));
		StopWaitingForMessages(InOwnerComp);
		
		InOwnerComp.GetAIOwner()->GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::NewRequest, MyMemory->MoveRequestID, EPathFollowingVelocityMode::Keep);
	}

	MyMemory->DistancingTargetLocation = NewTargetLocation;

	PreMoveTask(InOwnerComp, InNodeMemory);
	
	const EBTNodeResult::Type NodeResult = PerformMoveTask(InOwnerComp, RawMemory);
	if (NodeResult != EBTNodeResult::InProgress)
	{
		FinishLatentTask(InOwnerComp, NodeResult);
	}
}

