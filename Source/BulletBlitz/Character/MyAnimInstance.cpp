// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "BulletBlitzCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BulletBlitz/Weapons/Weapons.h"


void UMyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    BulletBlitzCharacter = Cast<ABulletBlitzCharacter>(TryGetPawnOwner());

}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (BulletBlitzCharacter == nullptr)
    {
        BulletBlitzCharacter = Cast<ABulletBlitzCharacter>(TryGetPawnOwner());
    }
    if (BulletBlitzCharacter == nullptr) return;

    FVector Velocity = BulletBlitzCharacter->GetVelocity();
    Velocity.Z = 0.f;
    Speed = Velocity.Size();

    bIsInAir = BulletBlitzCharacter->GetCharacterMovement()->IsFalling();
    bIsAccelerating = BulletBlitzCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
    bWeaponEquipped = BulletBlitzCharacter->IsWeaponEquipped();

    class AWeapons* EquippedWeapon;
    EquippedWeapon = BulletBlitzCharacter->GetEquippedWeapon();
    bIsCrouched = BulletBlitzCharacter->bIsCrouched;
    bAiming = BulletBlitzCharacter->IsAiming();
    TurningInPlace = BulletBlitzCharacter->GetTurningInPlace();

    FRotator AimRotation = BulletBlitzCharacter->GetBaseAimRotation();
    FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BulletBlitzCharacter->GetVelocity());
    YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;


    CharacterRotationLastFrame = CharacterRotation;
    CharacterRotation = BulletBlitzCharacter->GetActorRotation();
    const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
    const float Target = Delta.Yaw / DeltaTime;
    const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
    Lean = FMath::Clamp(Interp, -90.f, 90.f);

    AO_Yaw = BulletBlitzCharacter->GetAO_Yaw();
    AO_Pitch = BulletBlitzCharacter->GetAO_Pitch();

  

    if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BulletBlitzCharacter->GetMesh())
    {

        LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);

        FVector OutPosition;
        FRotator OutRotation;
        BulletBlitzCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
        LeftHandTransform.SetLocation(OutPosition);
        LeftHandTransform.SetRotation(FQuat(OutRotation));

        if (BulletBlitzCharacter->IsLocallyControlled())
        {
			bLocallyControlled = true;
            FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);

            FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BulletBlitzCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 20.f);
        }
		
    }
}

