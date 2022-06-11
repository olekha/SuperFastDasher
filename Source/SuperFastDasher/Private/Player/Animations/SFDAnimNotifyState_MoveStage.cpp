// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Animations/SFDAnimNotifyState_MoveStage.h"
#include "GameFramework/SFDCombatManagerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequenceBase.h"
#include "Engine/StaticMeshSocket.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SFDCharacter.h"

void USFDAnimNotifyState_MoveStage::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if(MeshComp != nullptr)
	{
		if(AActor* OwnerActor = MeshComp->GetOwner())
		{
			if(USFDCombatManagerComponent* CombatManagerComponent 
				= Cast<USFDCombatManagerComponent>(OwnerActor->GetComponentByClass(USFDCombatManagerComponent::StaticClass())))
			{
				CachedOwnerCombatManagerComponent = CombatManagerComponent;

			}
		}
	}
}

void USFDAnimNotifyState_MoveStage::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	CachedOwnerCombatManagerComponent = nullptr;
}

void USFDAnimNotifyState_MoveStage::SetCombatMoveStage(const ESFDAttackState InAttackStage)
{
	if(CachedOwnerCombatManagerComponent != nullptr)
	{
		CachedOwnerCombatManagerComponent->SetComboState(InAttackStage);
	}
}

void USFDAnimNotifyState_MoveStage_Beginning::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	SetCombatMoveStage(ESFDAttackState::Beginning);
}

void USFDAnimNotifyState_MoveStage_DealDamage::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	SetCombatMoveStage(ESFDAttackState::Damage);
}

void USFDAnimNotifyState_MoveStage_DealDamage::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	DamagedActors.Reset();
}

const FName StartDamageSocketName = "StartDamage";
const FName EndDamageSocketName = "EndDamage";

void USFDAnimNotifyState_MoveStage_DealDamage::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	DealDamage();
}

void USFDAnimNotifyState_MoveStage_DealDamage::DealDamage()
{
	if(CachedOwnerCombatManagerComponent == nullptr)
	{
		return;
	}

	UStaticMeshComponent* WeaponMeshComponent = CachedOwnerCombatManagerComponent->GetActiveWeaponComponent();

	if(WeaponMeshComponent == nullptr)
	{
		return;
	}

	const UStaticMeshSocket* StartDamageSocket = WeaponMeshComponent->GetSocketByName(StartDamageSocketName);
	const UStaticMeshSocket* EndDamageSocket = WeaponMeshComponent->GetSocketByName(EndDamageSocketName);

	if(!ensureMsgf(StartDamageSocket != nullptr && EndDamageSocket != nullptr, TEXT("Incorrect damage sockets setup on current weapon")))
	{
		return;
	}

	FTransform StartDamageSocketWorldTransform;
	StartDamageSocket->GetSocketTransform(StartDamageSocketWorldTransform, WeaponMeshComponent);
	
	FTransform EndDamageSocketWorldTransform;
	EndDamageSocket->GetSocketTransform(EndDamageSocketWorldTransform, WeaponMeshComponent);

	TArray<FHitResult> HitResults;

	if(UKismetSystemLibrary::SphereTraceMulti(CachedOwnerCombatManagerComponent,
		StartDamageSocketWorldTransform.GetLocation(),
		EndDamageSocketWorldTransform.GetLocation(),
		Radius,
		TraceChannel,
		bTraceComplex,
		DamagedActors,
		DrawDebugType,
		HitResults,
		bIgnoreSelf,
		TraceColor,
		TraceHitColor,
		DrawTime))
	{
		for(const FHitResult& HitResult : HitResults)
		{
			if(!HitResult.HasValidHitObjectHandle())
			{
				continue;
			}

			APawn* InstigatorPawn = WeaponMeshComponent->GetOwner<APawn>();
			AController* InstigatorController = InstigatorPawn->GetController();

			UGameplayStatics::ApplyDamage(HitResult.GetActor(), DamageToDeal, InstigatorController, InstigatorPawn, nullptr);

			DamagedActors.Add(HitResult.GetActor());
		}
	}
}

void USFDAnimNotifyState_MoveStage_Finish::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	SetCombatMoveStage(ESFDAttackState::Finish);
}

void USFDAnimNotifyState_MoveStage_Finish::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (CachedOwnerCombatManagerComponent == nullptr)
	{
		return;
	}

	//CachedOwnerCombatManagerComponent->StopCurrentSequence();

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
