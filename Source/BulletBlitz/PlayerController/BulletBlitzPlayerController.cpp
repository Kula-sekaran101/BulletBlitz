// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBlitzPlayerController.h"
#include "BulletBlitz/HUD/BulletBlitzHUD.h"
#include "BulletBlitz/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "BulletBlitz/Character/BulletBlitzCharacter.h"	


void ABulletBlitzPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BulletBlitzHUD = Cast<ABulletBlitzHUD>(GetHUD());

}


void ABulletBlitzPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	if (BulletBlitzHUD == nullptr)
	{
		BulletBlitzHUD = Cast<ABulletBlitzHUD>(GetHUD());
    }
	if (BulletBlitzHUD && BulletBlitzHUD->CharacterOverlay && BulletBlitzHUD->CharacterOverlay->HealthBar && BulletBlitzHUD->CharacterOverlay->HealthText)
	{
		const float HealthPercent = Health / MaxHealth;
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		BulletBlitzHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		BulletBlitzHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void ABulletBlitzPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ABulletBlitzCharacter* BulletBlitzCharacter = Cast<ABulletBlitzCharacter>(InPawn);
	if (BulletBlitzCharacter)
	{
		SetHUDHealth(BulletBlitzCharacter->GetHealth(), BulletBlitzCharacter->GetMaxHealth());
	}

}
