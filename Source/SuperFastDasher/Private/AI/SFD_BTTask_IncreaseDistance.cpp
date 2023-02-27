// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SFD_BTTask_IncreaseDistance.h"

#include "AIController.h"
#include "AI/SFDEnemyPawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/SFDCharacter.h"

USFD_BTTask_IncreaseDistance::USFD_BTTask_IncreaseDistance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "SFD Increase Distance";

	bShouldFacePlayer = true;
	bShouldTrackTargetLocationAlteration = false;

	DistanceToCover = 500.0f;
	bShouldFacePlayer = true;
}

EBTNodeResult::Type USFD_BTTask_IncreaseDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void USFD_BTTask_IncreaseDistance::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	const AAIController* const OwnerController = Cast<AAIController>(OwnerComp.GetOwner());
	const ASFDEnemyPawn* const OwnerPawn = Cast<ASFDEnemyPawn>(OwnerController->GetPawn());

	if(IsValid(OwnerPawn))
	{
		UCharacterMovementComponent* const OwnerMovementComponent = OwnerPawn->GetCharacterMovement();
		OwnerMovementComponent->SetMovementMode(EMovementMode::MOVE_NavWalking);
	}
}

void USFD_BTTask_IncreaseDistance::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if(bShouldFacePlayer)
	{
		KeepLookAtPlayerRotation(OwnerComp, NodeMemory, DeltaSeconds);
	}
}

FString USFD_BTTask_IncreaseDistance::GetStaticDescription() const
{
	return FString::Printf(TEXT("Distance owner AI from the player pawn for %.1f cm"), DistanceToCover);
}

FVector USFD_BTTask_IncreaseDistance::GetDistancingTargetLocation(const ASFDCharacter* const InPlayerPawn, const ASFDEnemyPawn* const InOwnerPawn) const
{
	if(!IsValid(InPlayerPawn)
		|| !IsValid(InOwnerPawn))
	{
		return FVector::ZeroVector;
	}
	
	const FVector& PlayerLocation = InPlayerPawn->GetActorLocation();
	const FVector& OwnerLocation = InOwnerPawn->GetActorLocation();

	const FVector DistancingDirection = (OwnerLocation - PlayerLocation).GetSafeNormal();

	const float RandomAngle = FMath::RandRange(0.0f, 360.0f);
	
	return FVector(DistanceToCover * FMath::Cos(RandomAngle), DistanceToCover * FMath::Sin(RandomAngle), OwnerLocation.Z+100);
}

void USFD_BTTask_IncreaseDistance::PreMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* const OwnerController = Cast<AAIController>(OwnerComp.GetOwner());
	const ASFDEnemyPawn* const OwnerPawn = Cast<ASFDEnemyPawn>(OwnerController->GetPawn());

	if(!IsValid(OwnerPawn))
	{
		return;
	}
	
	UCharacterMovementComponent* const OwnerMovementComponent = OwnerPawn->GetCharacterMovement();

	OwnerMovementComponent->bUseControllerDesiredRotation = !bShouldFacePlayer;
	OwnerMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
}

void USFD_BTTask_IncreaseDistance::KeepLookAtPlayerRotation(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory, float InDeltaSeconds)
{
	
}
