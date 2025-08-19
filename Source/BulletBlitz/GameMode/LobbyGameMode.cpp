// Fill out your copyright notice in the Description page of Project Settings.



// LobbyGameMode.cpp
#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "BulletBlitz/PlayerController/LobbyPlayerController.h"

ALobbyGameMode::ALobbyGameMode()
{
	// Ensure the lobby uses your custom PlayerController so it can create the lobby UI.
	PlayerControllerClass = ALobbyPlayerController::StaticClass();

	// Optional: seamless travel is handy for lobby -> match
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
		// Travel to your gameplay map; keep ?listen so server keeps accepting clients
		World->ServerTravel(TEXT("/Game/Maps/BulletBlitzMap?listen"));
	}
}


