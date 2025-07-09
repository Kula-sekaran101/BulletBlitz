// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BulletBlitzPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BULLETBLITZ_API ABulletBlitzPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void OnRep_Score() override;
	void AddToScore(float ScoreAmount);
private:
	class ABulletBlitzCharacter* Character;
	class ABulletBlitzPlayerController* Controller;
};
