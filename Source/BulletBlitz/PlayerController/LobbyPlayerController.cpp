// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPlayerController.h"
#include "BulletBlitz/GameMode/LobbyGameMode.h"
#include "Engine/World.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	bShowMouseCursor = false;

	
	EnableInput(this);
}

void ALobbyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	
	InputComponent->BindAction("StartMatch", IE_Pressed, this, &ALobbyPlayerController::StartMatch);
}

void ALobbyPlayerController::StartMatch()
{
	
	if (!HasAuthority()) return;

	if (ALobbyGameMode* LobbyGM = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode()))
	{
		LobbyGM->HostStartGame();
	}
}
