// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/SFDPlayerHealthBarWidget.h"
#include "../Public/GameFramework/SFDVitalityComponent.h"
#include "../Public/Player/SFDCharacter.h"
#include "Components/ProgressBar.h"

USFDPlayerHealthBarWidget::USFDPlayerHealthBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USFDPlayerHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (GetPlayerContext().IsValid())
	{
		ASFDCharacter* PlayerCharacter = GetPlayerContext().GetPawn<ASFDCharacter>();
		if (PlayerCharacter != nullptr)
		{
			USFDVitalityComponent* PlayerVitalityComponent = PlayerCharacter->GetVitalityComponent();
			if (PlayerVitalityComponent != nullptr)
			{
				if (!PlayerVitalityComponent->OnHealthChanged().IsBoundToObject(this))
				{
					PlayerVitalityComponent->OnHealthChanged().AddUObject(this, &USFDPlayerHealthBarWidget::OnHealthChanged);
				}
			}
		}
	}

	UpdateCurrentHealthValue();
}

void USFDPlayerHealthBarWidget::NativeDestruct()
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
				PlayerVitalityComponent->OnHealthChanged().RemoveAll(this);
			}
		}
	}
}

void USFDPlayerHealthBarWidget::OnHealthChanged(USFDVitalityComponent* InVitalityComponent)
{
	UpdateCurrentHealthValue();
}

void USFDPlayerHealthBarWidget::UpdateCurrentHealthValue()
{
	const float CurrentHealth = GetCurrentHealthValue();

	HealthBar_ProgressBar->SetPercent(CurrentHealth);
}

float USFDPlayerHealthBarWidget::GetCurrentHealthValue() const
{
	if (GetPlayerContext().IsValid())
	{
		ASFDCharacter* PlayerCharacter = GetPlayerContext().GetPawn<ASFDCharacter>();
		if (PlayerCharacter != nullptr)
		{
			USFDVitalityComponent* PlayerVitalityComponent = PlayerCharacter->GetVitalityComponent();
			if (PlayerVitalityComponent != nullptr)
			{
				return PlayerVitalityComponent->GetCurrentHealthPercents();
			}
		}
	}

	ensureAlways(false);

	UE_LOG(LogTemp, Error, TEXT("USFDPlayerHealthBarWidget::GetCurrentHealthValue: Can't retreive current health value for widget"));

	return 0.0f;
}
