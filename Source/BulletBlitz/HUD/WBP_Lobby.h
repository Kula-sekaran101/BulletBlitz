// Fill out your copyright notice in the Description page of Project Settings.

// WBP_Lobby.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WBP_Lobby.generated.h"

UCLASS()
class BULLETBLITZ_API UWBP_Lobby : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// In your widget blueprint (WBP_Lobby), the Button’s name MUST be exactly "StartButton"
	UPROPERTY(meta = (BindWidget))
	class UButton* StartButton;

	UFUNCTION()
	void OnStartButtonClicked();
};
