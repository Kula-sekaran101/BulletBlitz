// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BulletBlitzPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BULLETBLITZ_API ABulletBlitzPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	void SetHUDHealth(float Health, float MaxHealth);

protected:
	virtual void BeginPlay() override;

private:
	class ABulletBlitzHUD* BulletBlitzHUD;

	
};
