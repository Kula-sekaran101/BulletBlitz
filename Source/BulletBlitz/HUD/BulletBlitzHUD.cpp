// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBlitzHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include <BulletBlitz/Character/BulletBlitzCharacter.h>




void ABulletBlitzHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void ABulletBlitzHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		if (CharacterOverlay)
		{
			CharacterOverlay->AddToViewport();

		}
	}
}


void ABulletBlitzHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);


		float SpreadScale = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshairs(HUDPackage.CrosshairsCenter, ViewportCenter , Spread , HUDPackage.CrosshairColour);
		}
		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread( 0.f ,-SpreadScale);
			DrawCrosshairs(HUDPackage.CrosshairsTop, ViewportCenter , Spread , HUDPackage.CrosshairColour);
		}
		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScale, 0.f);
			DrawCrosshairs(HUDPackage.CrosshairsRight, ViewportCenter,Spread , HUDPackage.CrosshairColour);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScale, 0.f);
			DrawCrosshairs(HUDPackage.CrosshairsLeft, ViewportCenter ,Spread , HUDPackage.CrosshairColour);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f, SpreadScale);
			DrawCrosshairs(HUDPackage.CrosshairsBottom, ViewportCenter , Spread, HUDPackage.CrosshairColour);
		}
	}
}




void ABulletBlitzHUD::DrawCrosshairs(UTexture2D* Texture, FVector2D ViewportCenter , FVector2D Spread , FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f)+Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f, 0.f, 1.f, 1.f,
		CrosshairColor
	);
}
