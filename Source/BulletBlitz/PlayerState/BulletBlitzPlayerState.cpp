// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBlitzPlayerState.h"
#include "BulletBlitz/Character/BulletBlitzCharacter.h"
#include "BulletBlitz/PlayerController/BulletBlitzPlayerController.h"



void ABulletBlitzPlayerState::AddToScore(float ScoreAmount)
{
	Score += ScoreAmount;
	Character = Character == nullptr ? Cast<ABulletBlitzCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABulletBlitzPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(ScoreAmount);
		}
	}
}



void ABulletBlitzPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ABulletBlitzCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABulletBlitzPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(Score);
		}
	}

}

