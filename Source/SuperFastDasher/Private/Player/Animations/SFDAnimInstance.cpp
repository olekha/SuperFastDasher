// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Animations/SFDAnimInstance.h"

#include "GameFramework/Character.h"

void USFDAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	UpdateAnimationBlendAlpha();
	UpdateSpeed();
	UpdateDirection();
}

void USFDAnimInstance::UpdateAnimationBlendAlpha()
{
	AnimationBlendAlpha = bShoulUseWholeBody ? 0.0f : 1.0f; 
}

void USFDAnimInstance::UpdateSpeed()
{
	const ACharacter* const OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if(OwnerCharacter == nullptr)
	{
		return;
	}

	Speed = OwnerCharacter->GetVelocity().Length();
}

void USFDAnimInstance::UpdateDirection()
{
	const ACharacter* const OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
    if(OwnerCharacter == nullptr)
    {
    	Direction = 0.0f;
    	return;
    }

	const FRotator CharacterRotator = OwnerCharacter->GetActorRotation();
	const FVector VelocityVectorNormalized = OwnerCharacter->GetVelocity().GetSafeNormal();
	
	Direction = FMath::RadiansToDegrees(FMath::Atan2(VelocityVectorNormalized.X, VelocityVectorNormalized.Y));

	Direction += CharacterRotator.Yaw - 90.f;
	
	if(Direction > 180.0f)
	{
		Direction -= 360.0f;
	}
	else if(Direction < -180.0f)
	{
		Direction += 360.0f;	
	}
}
