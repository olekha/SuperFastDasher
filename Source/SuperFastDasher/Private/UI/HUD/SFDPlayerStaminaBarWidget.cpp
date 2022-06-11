// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SFDPlayerStaminaBarWidget.h"
#include "../Public/GameFramework/SFDVitalityComponent.h"
#include "../Public/Player/SFDCharacter.h"
#include "Components/ProgressBar.h"

USFDPlayerStaminaBarWidget::USFDPlayerStaminaBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USFDPlayerStaminaBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(GetPlayerContext().IsValid())
	{
		ASFDCharacter* PlayerCharacter = GetPlayerContext().GetPawn<ASFDCharacter>();
		if(PlayerCharacter != nullptr)
		{
			USFDVitalityComponent* PlayerVitalityComponent = PlayerCharacter->GetVitalityComponent();
			if(PlayerVitalityComponent != nullptr)
			{
				if(!PlayerVitalityComponent->OnStaminaChanged().IsBoundToObject(this))
				{
					PlayerVitalityComponent->OnStaminaChanged().AddUObject(this, &USFDPlayerStaminaBarWidget::OnStaminaChanged);
				}
			}
		}
	}

	UpdateCurrentStaminaValue();
}

void USFDPlayerStaminaBarWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (GetPlayerContext().IsValid())
	{
		ASFDCharacter* PlayerCharacter = GetPlayerContext().GetPawn<ASFDCharacter>();
		if (PlayerCharacter != nullptr)
		{
			USFDVitalityComponent* PlayerVitalityComponent = PlayerCharacter->GetVitalityComponent();
			if (PlayerVitalityComponent != nullptr)
			{
				PlayerVitalityComponent->OnStaminaChanged().RemoveAll(this);	
			}
		}
	}
}

void USFDPlayerStaminaBarWidget::OnStaminaChanged(USFDVitalityComponent* InVitalityComponent)
{
	UpdateCurrentStaminaValue();
}

void USFDPlayerStaminaBarWidget::UpdateCurrentStaminaValue()
{
	const float CurrentStamina = GetCurrentStaminaValue();

	StaminaBar_ProgressBar->SetPercent(CurrentStamina);
}

float USFDPlayerStaminaBarWidget::GetCurrentStaminaValue() const
{
	if (GetPlayerContext().IsValid())
	{
		ASFDCharacter* PlayerCharacter = GetPlayerContext().GetPawn<ASFDCharacter>();
		if (PlayerCharacter != nullptr)
		{
			USFDVitalityComponent* PlayerVitalityComponent = PlayerCharacter->GetVitalityComponent();
			if (PlayerVitalityComponent != nullptr)
			{
				return PlayerVitalityComponent->GetCurrentStaminaPercents();
			}
		}
	}

	ensure(false);

	UE_LOG(LogTemp, Error, TEXT("USFDPlayerStaminaBarWidget::GetCurrentStaminaValue: Can't retreive current stamina value for widget"));

	return 0.0f;
}
