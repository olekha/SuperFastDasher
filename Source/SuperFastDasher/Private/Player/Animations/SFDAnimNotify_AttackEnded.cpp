// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Animations/SFDAnimNotify_AttackEnded.h"
#include "Player/SFDCharacter.h"
#include "GameFramework/SFDCombatManagerComponent.h"

void USFDAnimNotify_AttackEnded::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp != nullptr)
	{
		if (AActor* OwnerActor = MeshComp->GetOwner())
		{
			if (USFDCombatManagerComponent* CombatManagerComponent
				= Cast<USFDCombatManagerComponent>(OwnerActor->GetComponentByClass(USFDCombatManagerComponent::StaticClass())))
			{
				CombatManagerComponent->StopCurrentSequence();
			}
		}
	}
}