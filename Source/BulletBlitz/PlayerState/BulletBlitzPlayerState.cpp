// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBlitzPlayerState.h"
#include "BulletBlitz/Character/BulletBlitzCharacter.h"
#include "BulletBlitz/PlayerController/BulletBlitzPlayerController.h"



void ABulletBlitzPlayerState::AddToScore(float ScoreAmount)
{
	Score += ScoreAmount;

	APawn* Pawn = GetPawn();
	if (!Pawn) return;

	ABulletBlitzCharacter* BlitzChar = Cast<ABulletBlitzCharacter>(Pawn);
	if (!BlitzChar) return;

	ABulletBlitzPlayerController* BlitzController = Cast<ABulletBlitzPlayerController>(BlitzChar->Controller);
	if (!BlitzController || !BlitzController->IsLocalController()) return;

	BlitzController->SetHUDScore(Score);
}




void ABulletBlitzPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	
	APawn* Pawn = GetPawn();
	if (!Pawn) return; 

	ABulletBlitzCharacter* BlitzChar = Cast<ABulletBlitzCharacter>(Pawn);
	if (!BlitzChar) return;

	ABulletBlitzPlayerController* BlitzController = Cast<ABulletBlitzPlayerController>(BlitzChar->Controller);
	if (!BlitzController || !BlitzController->IsLocalController()) return;


	BlitzController->SetHUDScore(Score);
}




