// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SFDEnemyPawn.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASFDEnemyPawn::ASFDEnemyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASFDEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASFDEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASFDEnemyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASFDEnemyPawn::SetState(const ESFDEnumAIState InNewState)
{
	if(CurrentState != InNewState)
	{
		PreviousState = CurrentState;
		CurrentState = InNewState;
	}
}

// void ASFDEnemyPawn::StartPlayerChase(const FVector& InPlayerLocation)
// {
// 	const FVector& OwnLocation = GetActorLocation();
//
// 	const float DistanceToPlayer = (InPlayerLocation - OwnLocation).Length();
//
// 	UCharacterMovementComponent* OwnMovementComponent = GetCharacterMovement();
//
// 	const EMovementMode NewMovementMode = DistanceToPlayer >= FlyToPlayerThresholdValue ? EMovementMode::MOVE_Flying : EMovementMode::MOVE_NavWalking;
// 	
// 	OwnMovementComponent->SetMovementMode(NewMovementMode);
// }
//
//
// void ASFDEnemyPawn::EndPlayerChase()
// {
// 	UCharacterMovementComponent* OwnMovementComponent = GetCharacterMovement();
// 	
// 	if(OwnMovementComponent->IsFlying())
// 	{
// 		OwnMovementComponent->SetMovementMode(EMovementMode::MOVE_NavWalking);
// 	}
// }

