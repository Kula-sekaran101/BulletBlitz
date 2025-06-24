// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "BulletBlitz/Weapons/Weapons.h"
#include "BulletBlitz/Character/BulletBlitzCharacter.h"	
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "BulletBlitz/PlayerController/BulletBlitzPlayerController.h"
//#include "BulletBlitz/HUD/BulletBlitzHUD.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"

UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    BaseWalkSpeed = 700.f;
    AimWalkSpeed = 450.f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UCombatComponent, EquippedWeapon);
    DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    /*PrimaryComponentTick.Target = this;
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.SetTickFunctionEnable(true);
    PrimaryComponentTick.RegisterTickFunction(GetComponentLevel());*/


    if (Character)
    {
        Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

        if (Character->GetFollowCamera())
        {
            DefaultFOV = Character->GetFollowCamera() -> FieldOfView;
            CurrentFOV = DefaultFOV;
        }
    }
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    
	if (Character && Character->IsLocallyControlled())
	{
        FHitResult HitResult;
        TraceUnderCrosshairs(HitResult);

        HitTarget = HitResult.ImpactPoint;
        SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
    

}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
    if (Character == nullptr || Character->Controller == nullptr) return;
	Controller = Controller == NULL ? Cast<ABulletBlitzPlayerController>(Character->GetController()) : Controller;
    if (Controller)
    {
        HUD = HUD == nullptr ? Cast<ABulletBlitzHUD>(Controller->GetHUD()): HUD;
        if (HUD)
        {

            
			if (EquippedWeapon)
			{
                
                HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
                HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
                HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
                HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
                HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
            }
            else
            {
              
                HUDPackage.CrosshairsCenter = nullptr;
                HUDPackage.CrosshairsBottom = nullptr;
                HUDPackage.CrosshairsLeft = nullptr;
                HUDPackage.CrosshairsRight = nullptr;
                HUDPackage.CrosshairsTop = nullptr; 
            }

			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplayerRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f; 

            CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplayerRange, Velocity.Size());


			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.5f);
			}
            else
            {
                CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
            }
            if (bAiming)
            {
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
            }
            else
            {
                CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
            }

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);

			HUDPackage.CrosshairSpread = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;

            HUD->SetHUDPackage(HUDPackage);
        }
    }
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
    if (EquippedWeapon == nullptr) return;

    if (bAiming)
    {
        CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
    }
    else
    {
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
    }

	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}




void UCombatComponent::SetAiming(bool bIsAiming)
{
    bAiming = bIsAiming;
    ServerSetAiming(bIsAiming);

    if (Character)
    {
        Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
    }
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
    bAiming = bIsAiming;

    if (Character)
    {
        Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
    }
}

void UCombatComponent::OnRep_EquippedWeapon()
{
    if (EquippedWeapon && Character)
    {
        Character->GetCharacterMovement()->bOrientRotationToMovement = false;
        Character->bUseControllerRotationYaw = true;
    }
}


void UCombatComponent::StartFireTimer()
{
    if (EquippedWeapon == nullptr || Character == nullptr) return;
    Character->GetWorldTimerManager().SetTimer(FireTimer, this, &UCombatComponent::FireTimerFinished, EquippedWeapon->FireDelay);
}

void UCombatComponent::FireTimerFinished()
{

    bCanFire = true;
    if (bFireButtonPressed  && EquippedWeapon->bAutomatic)
    {
        Fire();
    }
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
    bFireButtonPressed = bPressed;

    if (bFireButtonPressed && EquippedWeapon)
    {
        Fire();
    }
}

void UCombatComponent::Fire()
{
    if (bCanFire)
    {
        bCanFire = false;
        ServerFire(HitTarget);

        if (EquippedWeapon)
        {
            CrosshairShootingFactor = 0.75f;
        }
        StartFireTimer();
    }
   
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
    FVector2D ViewportSize;

    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
    }

    FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
    FVector CrosshairWorldPosition;
    FVector CrosshairWorldDirection;

    bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
        UGameplayStatics::GetPlayerController(this, 0),
        CrosshairLocation,
        CrosshairWorldPosition,
        CrosshairWorldDirection
    );

    if (bScreenToWorld)
    {
        FVector Start = CrosshairWorldPosition;
        if (Character)
        {
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f); 
        }
        FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

        GetWorld()->LineTraceSingleByChannel(
            TraceHitResult,
            Start,
            End,
            ECollisionChannel::ECC_Visibility
        );
        if (!TraceHitResult.bBlockingHit) TraceHitResult.ImpactPoint = End;
		/*if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairColour = FLinearColor::Red;
		}
        else
        {
			HUDPackage.CrosshairColour = FLinearColor::White;
        }*/
        HUDPackage.CrosshairColour = FLinearColor::White;

		AActor* HitActor = TraceHitResult.GetActor();
        if (HitActor && HitActor->IsA(ABulletBlitzCharacter::StaticClass()) && HitActor != GetOwner())
        {
            HUDPackage.CrosshairColour = FLinearColor::Red;
        }
        else
        {
            HUDPackage.CrosshairColour = FLinearColor::White;
        }
 
    }
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
    MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
    if (EquippedWeapon == nullptr) return;

    if (Character)
	{
		
        Character->PlayFireMontage(bAiming);
        EquippedWeapon->Fire(TraceHitTarget);
    }
}



void UCombatComponent::EquipWeapon(AWeapons* WeaponToEquip)
{
    if (Character == nullptr || WeaponToEquip == nullptr) return;

    EquippedWeapon = WeaponToEquip;
    EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

    const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
    if (HandSocket)
    {
        HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
    }

    EquippedWeapon->SetOwner(Character);
    Character->GetCharacterMovement()->bOrientRotationToMovement = false;
    Character->bUseControllerRotationYaw = true;
}
