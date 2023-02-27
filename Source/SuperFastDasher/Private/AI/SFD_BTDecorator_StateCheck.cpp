// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SFD_BTDecorator_StateCheck.h"

#include "AIController.h"
#include "AI/SFDEnemyPawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SuperFastDasher/SuperFastDasher.h"

USFD_BTDecorator_StateCheck::USFD_BTDecorator_StateCheck(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "SFD Check State";

	StateToPass = ESFDEnumAIState::CutDistance;
}

bool USFD_BTDecorator_StateCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* const OwnerController = Cast<AAIController>(OwnerComp.GetOwner());
	const ASFDEnemyPawn* const OwnerPawn = Cast<ASFDEnemyPawn>(OwnerController->GetPawn());

	if(IsValid(OwnerPawn))
	{
		return OwnerPawn->GetState() == StateToPass;
	}
	
	return false;
}

FString USFD_BTDecorator_StateCheck::GetStaticDescription() const
{
	// basic info: result after time
	return FString::Printf(TEXT("Is owner AI state is equal to : %s "), *SFD::EnumToString("ESFDEnumAIState", static_cast<uint8>(StateToPass)));
}