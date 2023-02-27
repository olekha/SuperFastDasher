// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SFD_BTTask_DecideState.h"

#include "AIController.h"
#include "AI/SFDEnemyPawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/SFDCharacter.h"

static bool flicker = true;

EBTNodeResult::Type USFD_BTTask_DecideState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* const OwnerController = Cast<AAIController>(OwnerComp.GetOwner());
	ASFDEnemyPawn* const OwnerPawn = Cast<ASFDEnemyPawn>(OwnerController->GetPawn());

	const UBlackboardComponent* const BlackboardComp = OwnerComp.GetBlackboardComponent();
	const ASFDCharacter* const PlayerPawn = Cast<ASFDCharacter>(BlackboardComp->GetValueAsObject(PlayerPwanBBKeySelector.SelectedKeyName));
	
	if(!IsValid(OwnerController)
		|| !IsValid(PlayerPawn)
		|| !IsValid(OwnerPawn))
	{
		return EBTNodeResult::Failed;
	}

	const uint8 NewState = FMath::RandRange(static_cast<uint8>(ESFDEnumAIState::None) + 1, static_cast<uint8>(ESFDEnumAIState::MAX) - 1);
	
	OwnerPawn->SetState(flicker ? ESFDEnumAIState::IncreaseDistancing : ESFDEnumAIState::CutDistance);

	flicker = !flicker;
	
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type USFD_BTTask_DecideState::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void USFD_BTTask_DecideState::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}
