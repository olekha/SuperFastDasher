// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFDPlayerStaminaBarWidget.generated.h"

class UProgressBar;
class USFDVitalityComponent;

UCLASS(HideDropdown)
class SUPERFASTDASHER_API USFDPlayerStaminaBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	USFDPlayerStaminaBarWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

private:

	void OnStaminaChanged(USFDVitalityComponent* InVitalityComponent);

	void UpdateCurrentStaminaValue();

	float GetCurrentStaminaValue() const;

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UProgressBar* StaminaBar_ProgressBar;

};
