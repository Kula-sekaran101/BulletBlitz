// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BBGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BULLETBLITZ_API UBBGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, Category = "Multiplayer")
	FString RoomCode;
	
};
