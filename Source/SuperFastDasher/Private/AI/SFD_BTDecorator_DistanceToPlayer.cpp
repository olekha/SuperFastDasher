// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SFD_BTDecorator_DistanceToPlayer.h"

#include "AIController.h"
#include "AI/SFDEnemyPawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Player/SFDCharacter.h"

USFD_BTDecorator_DistanceToPlayer::USFD_BTDecorator_DistanceToPlayer(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	NodeName = "SFD Distance To Player";

	ComparisonRuleToUse = ESFDComparisonRule::LessOrEqual;
	ThresholdDistance = 500.0f;
}

bool USFD_BTDecorator_DistanceToPlayer::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* const OwnerController = Cast<AAIController>(OwnerComp.GetOwner());
	const ASFDEnemyPawn* const OwnerPawn = Cast<ASFDEnemyPawn>(OwnerController->GetPawn());

	const UBlackboardComponent* const BlackboardComp = OwnerComp.GetBlackboardComponent();
	const ASFDCharacter* const PlayerPawn = Cast<ASFDCharacter>(BlackboardComp->GetValueAsObject(PlayerPwanBBKeySelector.SelectedKeyName));
	
	if(IsValid(OwnerPawn)
		&& IsValid(PlayerPawn))
	{
		const FVector& PlayerLocation = PlayerPawn->GetActorLocation();
		const FVector& OwnerLocation = OwnerPawn->GetActorLocation();

		switch (ComparisonRuleToUse)
		{
		case ESFDComparisonRule::LessOrEqual:
			return (PlayerLocation - OwnerLocation).Length() <= ThresholdDistance;
		case ESFDComparisonRule::Equal:
			return (PlayerLocation - OwnerLocation).Length() == ThresholdDistance;
		case ESFDComparisonRule::GreaterOrEqual:
			return (PlayerLocation - OwnerLocation).Length() >= ThresholdDistance;
		}
	}
	
	return false;
}

FString USFD_BTDecorator_DistanceToPlayer::GetStaticDescription() const
{
	switch (ComparisonRuleToUse)
	{
	case ESFDComparisonRule::LessOrEqual:
		return FString::Printf(TEXT("Is distance between owner AI and Player is less than %.1f cm"), ThresholdDistance);
	case ESFDComparisonRule::Equal:
		return FString::Printf(TEXT("Is distance between owner AI and Player is equal to %.1f cm"), ThresholdDistance);
	case ESFDComparisonRule::GreaterOrEqual:
		return FString::Printf(TEXT("Is distance between owner AI and Player is greater than %.1f cm"), ThresholdDistance);
	}
	
	return FString::Printf(TEXT("Compare distance between owner AI and Player"));
}
