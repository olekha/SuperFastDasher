// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFDPlayerHealthBarWidget.generated.h"

class UProgressBar;
class USFDVitalityComponent;

UCLASS()
class SUPERFASTDASHER_API USFDPlayerHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	USFDPlayerHealthBarWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

private:

	void OnHealthChanged(USFDVitalityComponent* InVitalityComponent);

	float GetCurrentHealthValue() const;

	void UpdateCurrentHealthValue();

private:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UProgressBar* HealthBar_ProgressBar;

};
