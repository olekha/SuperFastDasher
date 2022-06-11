// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SFDAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SUPERFASTDASHER_API USFDAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	void SetShouldUseWholeBody(const bool bInShoulUseWholeBody)
	{
		bShoulUseWholeBody = bInShoulUseWholeBody;
	}

private:
	void UpdateAnimationBlendAlpha();

	void UpdateSpeed();

	void UpdateDirection();
	
private:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bShoulUseWholeBody = false;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	float Direction = 0.0f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	float AnimationBlendAlpha = 0.0f;
};
