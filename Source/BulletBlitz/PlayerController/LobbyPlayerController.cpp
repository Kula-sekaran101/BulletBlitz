// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPlayerController.h"
#include "BulletBlitz/GameMode/LobbyGameMode.h"
#include "Engine/World.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Optional: show cursor in lobby
	bShowMouseCursor = true;

	// Enable input so we can receive key presses
	EnableInput(this);
}

void ALobbyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Bind a key (Enter) to start match
	InputComponent->BindAction("StartMatch", IE_Pressed, this, &ALobbyPlayerController::StartMatch);
}

void ALobbyPlayerController::StartMatch()
{
	// Only host can start the match
	if (!HasAuthority()) return;

	if (ALobbyGameMode* LobbyGM = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode()))
	{
		LobbyGM->HostStartGame();
	}
}
