// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BulletBlitz/HUD/BulletBlitzHUD.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BULLETBLITZ_API UCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatComponent();
    friend class ABulletBlitzCharacter;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void EquipWeapon(class AWeapons* WeaponToEquip);
    void FireButtonPressed(bool bPressed);

protected:
    virtual void BeginPlay() override;
    void SetAiming(bool bIsAiming);
    void TraceUnderCrosshairs(FHitResult& TraceHitResult);

    UFUNCTION(Server, Reliable)
    void ServerSetAiming(bool bIsAiming);

    UFUNCTION(Server, Reliable)
    void ServerFire(const FVector_NetQuantize& TraceHitTarget);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

    UFUNCTION()
    void OnRep_EquippedWeapon();

	void SetHUDCrosshairs(float DeltaTime);

private:
    class ABulletBlitzCharacter* Character;
	class ABulletBlitzPlayerController* Controller;
	class ABulletBlitzHUD* HUD;

    UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
    AWeapons* EquippedWeapon;

    UPROPERTY(Replicated)
    bool bAiming;

    UPROPERTY(EditAnywhere)
    float BaseWalkSpeed;

    UPROPERTY(EditAnywhere)
    float AimWalkSpeed;

    bool bFireButtonPressed;

	FVector HitTarget;

    void Fire();

    /*
	* Hud and Crosshairs
    */

    float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

    FHUDPackage HUDPackage;

    /*
    Aiming anf FOV
    */

    float DefaultFOV;

    UPROPERTY(EditAnywhere , Category = Combat)
    float ZoomedFOV = 30.f;

    float CurrentFOV;

    UPROPERTY(EditAnywhere, Category = Combat)
    float ZoomInterpSpeed = 20.f;

    void InterpFOV(float DeltaTime);
    
    /*
    * automatic fire
    */

	FTimerHandle FireTimer;

    bool bCanFire = true;

    void FireTimerFinished();
	void StartFireTimer();
};