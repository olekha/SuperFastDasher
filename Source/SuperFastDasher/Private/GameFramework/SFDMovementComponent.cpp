// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/SFDMovementComponent.h"

void USFDMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!Velocity.IsNearlyZero())
	{
		LastNonZeroVelocity = Velocity;
	}
}
