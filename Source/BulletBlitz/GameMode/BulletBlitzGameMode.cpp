// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBlitzGameMode.h"
#include "BulletBlitz/Character/BulletBlitzCharacter.h"
#include <BulletBlitz/BulletBlitzComponent/CombatComponent.h>
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include <BulletBlitz/PlayerState/BulletBlitzPlayerState.h>
#include "BulletBlitz/PlayerController/BulletBlitzPlayerController.h"  

void ABulletBlitzGameMode::PlayerEliminated(ABulletBlitzCharacter* ElimedCharacter, ABulletBlitzPlayerController* VictimController, ABulletBlitzPlayerController* AttackerController)
{
	ABulletBlitzPlayerState* AttackerPlayerState = AttackerController ? Cast<ABulletBlitzPlayerState>(AttackerController->PlayerState) : nullptr;
	ABulletBlitzPlayerState* VictimPlayerState = VictimController ? Cast<ABulletBlitzPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && VictimPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}
	if (ElimedCharacter)
	{
		ElimedCharacter->Elim();
	}
}

void ABulletBlitzGameMode::RequestRespawn(ACharacter* ElimedCharacter, AController* ElimmedControlle)
{
	if (ElimedCharacter)
	{
		ElimedCharacter->Reset();
		ElimedCharacter->Destroy();
	}
	if (ElimmedControlle)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedControlle  , PlayerStarts[Selection]);
	}
}
