// Fill out your copyright notice in the Description page of Project Settings.



#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "BulletBlitz/PlayerController/LobbyPlayerController.h"

ALobbyGameMode::ALobbyGameMode()
{
	// Use custom PlayerController
	PlayerControllerClass = ALobbyPlayerController::StaticClass();
	bUseSeamlessTravel = true;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GameState)
	{
		 NumPlayers = GameState->PlayerArray.Num();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 5.f, FColor::Green,
				FString::Printf(TEXT("Players in lobby: %d"), NumPlayers)
			);
		}
	}
}

void ALobbyGameMode::HostStartGame()
{
	if (!HasAuthority()) return;

	if (UWorld* World = GetWorld())
	{
		World->ServerTravel(TEXT("/Game/Maps/BulletBlitzMap?listen"));
	}
}
