// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BULLETBLITZ_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	 void PostLogin(APlayerController* NewPlayer) override;
	
};
