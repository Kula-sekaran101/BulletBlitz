// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BulletBlitzGameMode.generated.h"


/**
 * 
 */
UCLASS()
class BULLETBLITZ_API ABulletBlitzGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	void PlayerEliminated(class ABulletBlitzCharacter* ElimedCharacter , class ABulletBlitzPlayerController* VictimController, class ABulletBlitzPlayerController* AttackerController);
	
};
