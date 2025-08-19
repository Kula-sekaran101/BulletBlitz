// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Players in lobby: %d"), NumberOfPlayers));
	}

	if (NumberOfPlayers == 2) 
	{
		UWorld* World = GetWorld();
		if (World)
		{
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/Maps/BulletBlitzMap?listen"));
		}
	}
}

void ALobbyGameMode::BeginPlay()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("lobby game mode entered:")));
	}

	Super::BeginPlay();

	if (IsLocalController() && HasAuthority()) 
	{
		if (WBP_Lobby) 
		{
			UUserWidget* LobbyWidget = CreateWidget<UUserWidget>(this, LobbyWidgetClass);
			if (LobbyWidget)
			{
				LobbyWidget->AddToViewport();
			}
		}
	}

}

