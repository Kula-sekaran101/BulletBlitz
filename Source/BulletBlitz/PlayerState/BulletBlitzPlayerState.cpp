#include "BulletBlitzPlayerState.h"
#include "BulletBlitz/Character/BulletBlitzCharacter.h"
#include "BulletBlitz/PlayerController/BulletBlitzPlayerController.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"

void ABulletBlitzPlayerState::AddToScore(float ScoreAmount)
{
	Score += ScoreAmount;

	if (!Character)
	{
		Character = Cast<ABulletBlitzCharacter>(GetPawn());
	}

	if (Character)
	{
		if (!Controller)
		{
			Controller = Cast<ABulletBlitzPlayerController>(Character->GetController());
		}

		if (Controller)
		{
			Controller->SetHUDScore(Score);
		}
	}
}

void ABulletBlitzPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	if (!Character)
	{
		Character = Cast<ABulletBlitzCharacter>(GetPawn());
	}

	if (Character)
	{
		// Delay updating HUD in case controller is not yet ready on client
		FTimerHandle TimerHandle;
		Character->GetWorldTimerManager().SetTimer(
			TimerHandle,
			[this]()
			{
				if (!Character) return;

				if (!Controller)
				{
					Controller = Cast<ABulletBlitzPlayerController>(Character->GetController());
				}

				if (Controller)
				{
					Controller->SetHUDScore(Score);
				}
			},
			0.1f,  // delay to ensure controller & HUD are initialized
			false
		);
	}
}
