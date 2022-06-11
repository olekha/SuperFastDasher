// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFDVitalityComponent.generated.h"

UENUM()
enum class ESFDRegenerationRule : uint8
{
	None								UMETA(Hidden),
	ValueLessThenMaxValue,
	ValueBelowSpecifiedThreshold,
	MAX									UMETA(Hidden)
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPERFASTDASHER_API USFDVitalityComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(USFDVitalityComponent, FVitalityComponentEvent, USFDVitalityComponent*)

public:	
	// Sets default values for this component's properties
	USFDVitalityComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetHealth(int32 InNewAmount, bool bResetRegeneration = true, bool bForcedSet = false);

	UFUNCTION(BlueprintCallable)
	void AddHealth(int32 InAmount, bool bResetRegeneration = true);

	UFUNCTION(BlueprintCallable)
	void SetStamina(int32 InNewAmount, bool bResetRegeneration = true, bool bForcedSet = false);

	UFUNCTION(BlueprintCallable)
	void AddStamina(int32 InAmount, bool bResetRegeneration = true);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurrentHealth() const
	{
		return CurrentHealth;
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentHealthPercents() const
	{
		return static_cast<float>(CurrentHealth) / static_cast<float>(MaxHealth);
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurrentStamina() const
	{
		return CurrentStamina;
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentStaminaPercents() const
	{
		return static_cast<float>(CurrentStamina) / static_cast<float>(MaxStamina);
	}

	FVitalityComponentEvent& OnHealthChanged() { return HealthChangedEvent; }
	FVitalityComponentEvent& OnHealthRegenerated() { return HealthRegeneratedEvent; }
	FVitalityComponentEvent& OnHealthRegenerationStarted() { return HealthRegenerationStartedEvent; }
	FVitalityComponentEvent& OnHealthRegenerationStopped() { return HealthRegenerationStoppedEvent; }
	
	FVitalityComponentEvent& OnStaminaChanged() { return StaminaChangedEvent; }
	FVitalityComponentEvent& OnStaminaRegenerated() { return StaminaRegeneratedEvent; }
	FVitalityComponentEvent& OnStaminaRegenerationStarted() { return StaminaRegenerationStartedEvent; }
	FVitalityComponentEvent& OnStaminaRegenerationStopped() { return StaminaRegenerationStoppedEvent; }

	bool IsHealthRegenerationInProgress() const;
	bool IsHealthRegenerationRequired() const;

	bool IsStaminaRegenerationInProgress() const;
	bool IsStaminaRegenerationRequired() const;

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

private:

	void CheckForHealthRegeneration(bool bResetIfRegeneratingAlready = true);
	void CheckForStaminaRegeneration(bool bResetIfRegeneratingAlready = true);

	void StartHealthRegeneration();
	void StopHealthRegeneration();
	void HealthRegenerationIteration();

	void StartStaminaRegeneration();
	void StopStaminaRegeneration();
	void StaminaRegenerationIteration();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Health")
	bool bHasHealth = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasHealth", DisplayAfter = bHasHealth, ClampMin = "1"), Category = "Health")
	int32 MaxHealth = 100.f;

	UPROPERTY(Transient)
	int32 CurrentHealth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasHealth", DisplayAfter = bHasHealth), Category = "Health")
	bool bHasHealthRegeneration = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasHealth && bHasHealthRegeneration", DisplayAfter = bHasHealthRegeneration), Category = "Health")
	float HealthRegenerationsIntervals = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasHealth && bHasHealthRegeneration", DisplayAfter = bHasHealthRegeneration), Category = "Health")
	int32 HealthRegenerationsAmountPerIteration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasHealth && bHasHealthRegeneration", DisplayAfter = bHasHealthRegeneration), Category = "Health")
	float DelayBeforStartHealthRegeneration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasHealth && bHasHealthRegeneration", DisplayAfter = bHasHealthRegeneration), Category = "Health")
	ESFDRegenerationRule HealthRegenerationRule = ESFDRegenerationRule::ValueLessThenMaxValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasHealth && bHasHealthRegeneration && HealthRegenerationRule == ESFDRegenerationRule::ValueBelowSpecifiedThreshold", DisplayAfter = HealthRegenerationRule), Category = "Health")
	float HealthRegenerationThreshold = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Stamina")
	bool bHasStamina = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasStamina", DisplayAfter = bHasStamina, ClampMin = "1"), Category = "Stamina")
	int32 MaxStamina = 100.f;

	UPROPERTY(Transient)
	int32 CurrentStamina = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasStamina", DisplayAfter = bHasStamina), Category = "Stamina")
	bool bHasStaminaRegeneration = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasStamina && bHasStaminaRegeneration", DisplayAfter = bHasStaminaRegeneration), Category = "Stamina")
	float StaminaRegenerationsIntervals = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasStamina && bHasStaminaRegeneration", DisplayAfter = bHasStaminaRegeneration), Category = "Stamina")
	int32 StaminaRegenerationsAmountPerIteration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasStamina && bHasStaminaRegeneration", DisplayAfter = bHasStaminaRegeneration), Category = "Stamina")
	float DelayBeforStartStaminaRegeneration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasStamina && bHasStaminaRegeneration", DisplayAfter = bHasHealthRegeneration), Category = "Stamina")
	ESFDRegenerationRule StaminaRegenerationRule = ESFDRegenerationRule::ValueLessThenMaxValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", EditConditionHides, EditCondition = "bHasStamina && bHasStaminaRegeneration && StaminaRegenerationRule == ESFDRegenerationRule::ValueBelowSpecifiedThreshold", DisplayAfter = StaminaRegenerationRule), Category = "Stamina")
	float StaminaRegenerationThreshold = 80.0f;

	bool bIsFirstHealthRegenerationAfterStart = true;
	bool bIsFirstStaminaRegenerationAfterStart = true;

	FTimerHandle HealthRegenerationTimer;
	FTimerHandle StatminaRegeneraionTimer;

	FVitalityComponentEvent HealthChangedEvent;	
	FVitalityComponentEvent HealthRegeneratedEvent;
	FVitalityComponentEvent HealthRegenerationStartedEvent;
	FVitalityComponentEvent HealthRegenerationStoppedEvent;

	FVitalityComponentEvent StaminaChangedEvent;
	FVitalityComponentEvent StaminaRegeneratedEvent;
	FVitalityComponentEvent StaminaRegenerationStartedEvent;
	FVitalityComponentEvent StaminaRegenerationStoppedEvent;
};
