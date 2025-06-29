// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBlitzGameMode.h"
#include "BulletBlitz/Character/BulletBlitzCharacter.h"
#include <BulletBlitz/BulletBlitzComponent/CombatComponent.h>


void ABulletBlitzGameMode::PlayerEliminated(ABulletBlitzCharacter* ElimedCharacter, ABulletBlitzPlayerController* VictimController, ABulletBlitzPlayerController* AttackerController)
{
	if (ElimedCharacter)
	{
		ElimedCharacter->Elim();
	}
}
