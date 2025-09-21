// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BulletBlitzGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BULLETBLITZ_API UBulletBlitzGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UBulletBlitzGameInstance();

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Room")
    FString RoomCode;

    UFUNCTION(BlueprintCallable, Category = "Room")
    void SetRoomCode(const FString& NewRoomCode);

    UFUNCTION(BlueprintCallable, Category = "Room")
    FString GetRoomCode() const;
};
