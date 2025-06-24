// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Casing.generated.h"

UCLASS()
class BULLETBLITZ_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	

	ACasing();


private:

	

	UPROPERTY(EditAnywhere, Category = "Casing")
	class UBoxComponent* CasingBox;

	UPROPERTY(EditAnywhere, Category = "Casing")
	float ShellEjectionImpulse;

	UPROPERTY(EditAnywhere, Category = "Casing")
	class USoundCue* CasingSound;
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


public:	
	

};
