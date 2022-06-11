// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/SFDVitalityComponent.h"

// Sets default values for this component's properties
USFDVitalityComponent::USFDVitalityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	bIsFirstHealthRegenerationAfterStart = true;
	bIsFirstStaminaRegenerationAfterStart = true;
}

// Called when the game starts
void USFDVitalityComponent::BeginPlay()
{
	Super::BeginPlay();

	if(bHasHealth)
	{
		SetHealth(MaxHealth);
	}

	if(bHasStamina)
	{
		SetStamina(MaxStamina);
	}
}

// Called every frame
void USFDVitalityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USFDVitalityComponent::SetHealth(int32 InNewAmount, bool bResetRegeneration/* = true*/, bool bForcedSet /*= false*/)
{
	if(!bHasHealth)
	{
		return;
	}

	if(InNewAmount != CurrentHealth || bForcedSet)
	{
		CurrentHealth = InNewAmount;

		HealthChangedEvent.Broadcast(this);

		CheckForHealthRegeneration(bResetRegeneration);
	}
}

void USFDVitalityComponent::AddHealth(int32 InAmount, bool bResetRegeneration/* = true*/)
{
	if (!bHasHealth)
	{
		return;
	}

	const int32 NewHealthAmount = FMath::Clamp(CurrentHealth + InAmount, 0, MaxHealth);

	if(CurrentHealth != NewHealthAmount)
	{
		SetHealth(NewHealthAmount, bResetRegeneration);
	}
}

void USFDVitalityComponent::CheckForHealthRegeneration(bool bResetIfRegeneratingAlready/* = true*/)
{
	if(!bHasHealth || !bHasHealthRegeneration)
	{
		if(IsHealthRegenerationInProgress())	
		{
			StopHealthRegeneration();
		}

		return;
	}

	if(IsHealthRegenerationRequired())
	{
		if (bResetIfRegeneratingAlready 
			|| !IsHealthRegenerationInProgress())
		{
			StartHealthRegeneration();
		}
	}
	else
	{
		if(IsHealthRegenerationInProgress())
		{
			StopHealthRegeneration();
		}
	}
}

bool USFDVitalityComponent::IsHealthRegenerationInProgress() const
{
	UWorld* World = GetWorld();
	if(World != nullptr)
	{
		return World->GetTimerManager().IsTimerActive(HealthRegenerationTimer);
	}

	return false;
}

bool USFDVitalityComponent::IsHealthRegenerationRequired() const
{
	if (!bHasHealth)
	{
		return false;
	}

	switch (HealthRegenerationRule)
	{
	case ESFDRegenerationRule::ValueLessThenMaxValue:
		return  CurrentHealth < MaxHealth;
	case ESFDRegenerationRule::ValueBelowSpecifiedThreshold:
		return  CurrentHealth < HealthRegenerationThreshold;
	}

	return false;
}

void USFDVitalityComponent::StartHealthRegeneration()
{
	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		World->GetTimerManager().SetTimer(HealthRegenerationTimer, this, &USFDVitalityComponent::HealthRegenerationIteration, HealthRegenerationsIntervals, true, DelayBeforStartHealthRegeneration);
	}
}

void USFDVitalityComponent::StopHealthRegeneration()
{
	if(IsHealthRegenerationInProgress())
	{
		UWorld* World = GetWorld();
		if (World != nullptr)
		{
			World->GetTimerManager().ClearTimer(HealthRegenerationTimer);
		}

		bIsFirstHealthRegenerationAfterStart = true;

		if (HealthRegenerationStoppedEvent.IsBound())
		{
			HealthRegenerationStoppedEvent.Broadcast(this);
		}
	}
}

void USFDVitalityComponent::HealthRegenerationIteration()
{
	if(bIsFirstHealthRegenerationAfterStart)
	{
		bIsFirstHealthRegenerationAfterStart = false;
	
		if (HealthRegenerationStartedEvent.IsBound())
		{
			HealthRegenerationStartedEvent.Broadcast(this);
		}
	}

	const int32 PrevHealthValue = CurrentHealth;

	AddHealth(HealthRegenerationsAmountPerIteration, false);

	if(PrevHealthValue != CurrentHealth)
	{
		if (HealthRegeneratedEvent.IsBound())
		{
			HealthRegeneratedEvent.Broadcast(this);
		}
	}
}

void USFDVitalityComponent::SetStamina(int32 InNewAmount, bool bResetRegeneration /*= true*/, bool bForcedSet /*= false*/)
{
	if (!bHasStamina)
	{
		return;
	}

	if (InNewAmount != CurrentStamina || bForcedSet)
	{
		CurrentStamina = InNewAmount;

		StaminaChangedEvent.Broadcast(this);

		CheckForStaminaRegeneration(bResetRegeneration);
	}
}

void USFDVitalityComponent::AddStamina(int32 InAmount, bool bResetRegeneration/* = true*/)
{
	if (!bHasStamina)
	{
		return;
	}

	const int32 NewStaminaAmount = FMath::Clamp(CurrentStamina + InAmount, 0, MaxStamina);

	if (CurrentStamina != NewStaminaAmount)
	{
		SetStamina(NewStaminaAmount, bResetRegeneration);
	}
}

void USFDVitalityComponent::CheckForStaminaRegeneration(bool bResetIfRegeneratingAlready/* = true*/)
{
	if (!bHasStamina || !bHasStaminaRegeneration)
	{
		if (IsStaminaRegenerationInProgress())
		{
			StopStaminaRegeneration();
		}

		return;
	}

	if (IsStaminaRegenerationRequired())
	{
		if (bResetIfRegeneratingAlready 
			|| !IsStaminaRegenerationInProgress())
		{
			StartStaminaRegeneration();
		}
	}
	else
	{
		if (IsStaminaRegenerationInProgress())
		{
			StopStaminaRegeneration();
		}
	}
}

bool USFDVitalityComponent::IsStaminaRegenerationInProgress() const
{
	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		return World->GetTimerManager().IsTimerActive(StatminaRegeneraionTimer);
	}

	return false;
}

bool USFDVitalityComponent::IsStaminaRegenerationRequired() const
{
	if(!bHasStamina)
	{
		return false;
	}

	switch (StaminaRegenerationRule)
	{
	case ESFDRegenerationRule::ValueLessThenMaxValue:
		return  CurrentStamina < MaxStamina;
	case ESFDRegenerationRule::ValueBelowSpecifiedThreshold:
		return  CurrentStamina < StaminaRegenerationThreshold;
	}

	return false;
}

void USFDVitalityComponent::StartStaminaRegeneration()
{
	if (!bHasStamina)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		World->GetTimerManager().SetTimer(StatminaRegeneraionTimer, this, &USFDVitalityComponent::StaminaRegenerationIteration, StaminaRegenerationsIntervals, true, DelayBeforStartStaminaRegeneration);
	}
}

void USFDVitalityComponent::StopStaminaRegeneration()
{
	if(IsStaminaRegenerationInProgress())
	{
		UWorld* World = GetWorld();
		if (World != nullptr)
		{
			World->GetTimerManager().ClearTimer(StatminaRegeneraionTimer);
		}

		bIsFirstStaminaRegenerationAfterStart = false;

		if (StaminaRegenerationStoppedEvent.IsBound())
		{
			StaminaRegenerationStoppedEvent.Broadcast(this);
		}
	}
}

void USFDVitalityComponent::StaminaRegenerationIteration()
{
	if (bIsFirstStaminaRegenerationAfterStart)
	{
		bIsFirstStaminaRegenerationAfterStart = false;

		if(StaminaRegenerationStartedEvent.IsBound())
		{
			StaminaRegenerationStartedEvent.Broadcast(this);
		}
	}

	const int32 PrevStaminaValue = CurrentStamina;

	AddStamina(StaminaRegenerationsAmountPerIteration, false);

	if (PrevStaminaValue != CurrentStamina)
	{
		if(StaminaRegeneratedEvent.IsBound())
		{
			StaminaRegeneratedEvent.Broadcast(this);
		}
	}
}
