// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SFDAnimNotifyState_MoveStage.generated.h"

class USFDCombatManagerComponent;
class USkeletalMeshComponent;
class UAnimSequenceBase;
enum class ESFDAttackState: uint8;

UCLASS(Abstract)
class SUPERFASTDASHER_API USFDAnimNotifyState_MoveStage : public UAnimNotifyState
{
	GENERATED_BODY()

protected:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual void SetCombatMoveStage(const ESFDAttackState InAttackStage);

protected:

	UPROPERTY(Transient)
	USFDCombatManagerComponent* CachedOwnerCombatManagerComponent;
};

UCLASS(Blueprintable)
class SUPERFASTDASHER_API USFDAnimNotifyState_MoveStage_Beginning : public USFDAnimNotifyState_MoveStage
{
	GENERATED_BODY()

protected:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
};

extern const FName StartDamageSocketName;
extern const FName EndDamageSocketName;

UCLASS(Blueprintable)
class SUPERFASTDASHER_API USFDAnimNotifyState_MoveStage_DealDamage : public USFDAnimNotifyState_MoveStage
{
	GENERATED_BODY()

protected:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

private:

	void DealDamage();

private:

	UPROPERTY(Transient)
	TArray<AActor*> DamagedActors;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowedPriveAccess = "true"), Category = "Damage Trace Params")
	float Radius = 15.0f;

	UPROPERTY(EditDefaultsOnly, meta = (AllowedPriveAccess = "true"), Category = "Damage Trace Params")
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery2;

	UPROPERTY(EditDefaultsOnly, meta = (AllowedPriveAccess = "true"), Category = "Damage Trace Params")
	bool bTraceComplex = false;

	UPROPERTY(EditDefaultsOnly, meta = (AllowedPriveAccess = "true"), Category = "Damage Trace Params")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::ForDuration;

	UPROPERTY(EditDefaultsOnly, meta = (AllowedPriveAccess = "true"), Category = "Damage Trace Params")
	bool bIgnoreSelf = true;

	UPROPERTY(EditDefaultsOnly, meta = (AllowedPriveAccess = "true"), Category = "Damage Trace Params")
	FLinearColor TraceColor = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, meta = (AllowedPriveAccess = "true"), Category = "Damage Trace Params")
	FLinearColor TraceHitColor = FLinearColor::Green;

	UPROPERTY(EditDefaultsOnly, meta = (AllowedPriveAccess = "true"), Category = "Damage Trace Params")
	float DrawTime = .5f;

	UPROPERTY(EditDefaultsOnly, meta = (AllowedPriveAccess = "true"))
	float DamageToDeal = 15.0f;
};

UCLASS(Blueprintable)
class SUPERFASTDASHER_API USFDAnimNotifyState_MoveStage_Finish : public USFDAnimNotifyState_MoveStage
{
	GENERATED_BODY()

protected:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};