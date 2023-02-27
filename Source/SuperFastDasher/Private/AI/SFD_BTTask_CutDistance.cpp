// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SFD_BTTask_CutDistance.h"

#include "AIController.h"
#include "AI/SFDEnemyPawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/SFDCharacter.h"

USFD_BTTask_CutDistance::USFD_BTTask_CutDistance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "SFD Cut Distance";
}

EBTNodeResult::Type USFD_BTTask_CutDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

FVector USFD_BTTask_CutDistance::GetDistancingTargetLocation(const ASFDCharacter* const InPlayerPawn, const ASFDEnemyPawn* const InOwnerPawn) const
{
	if(!IsValid(InPlayerPawn))
	{
		return FVector::ZeroVector;
	}
	
	const FVector& TargetLocation = InPlayerPawn->GetActorLocation();
	
	return TargetLocation;
}

FString USFD_BTTask_CutDistance::GetStaticDescription() const
{
	return FString("Get owner AI closer to the player pawn");
}

void USFD_BTTask_CutDistance::PreMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* const OwnerController = Cast<AAIController>(OwnerComp.GetOwner());
	const ASFDEnemyPawn* const OwnerPawn = Cast<ASFDEnemyPawn>(OwnerController->GetPawn());

	if(!IsValid(OwnerPawn))
	{
		return;
	}
		
	UCharacterMovementComponent* const OwnerMovementComponent = OwnerPawn->GetCharacterMovement();

	OwnerMovementComponent->bUseControllerDesiredRotation = true;
}
