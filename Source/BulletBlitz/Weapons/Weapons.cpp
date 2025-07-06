// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "BulletBlitz/Character/BulletBlitzCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "BulletBlitz/Weapons/Casing.h"
#include "Engine/SkeletalMeshSocket.h"

AWeapons::AWeapons()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    SetRootComponent(WeaponMesh);

    WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
    AreaSphere->SetupAttachment(RootComponent);
    AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
    PickupWidget->SetupAttachment(RootComponent);
}

void AWeapons::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
        AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapons::OnSphereOverlap);
        AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapons::OnSphereEndOverlap);
    }

    if (PickupWidget)
    {
        PickupWidget->SetVisibility(false);
    }
}

void AWeapons::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

   
}

void AWeapons::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AWeapons, WeaponState);
}

void AWeapons::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABulletBlitzCharacter* Character = Cast<ABulletBlitzCharacter>(OtherActor);
    if (Character)
    {
        Character->SetOverlappingWeapon(this);
    }
}

void AWeapons::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ABulletBlitzCharacter* Character = Cast<ABulletBlitzCharacter>(OtherActor);
    if (Character)
    {
        Character->SetOverlappingWeapon(nullptr);
    }
}

void AWeapons::SetWeaponState(EWeaponState State)
{
    WeaponState = State;

    switch (WeaponState)
    {
    case EWeaponState::EWS_Equipped:
        ShowPickupWidget(false);
        AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WeaponMesh->SetSimulatePhysics(false);
        WeaponMesh->SetEnableGravity(false);
        WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        break;
    case EWeaponState::EWS_Dropped:
        if (HasAuthority())
        {
            AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        }
        WeaponMesh->SetSimulatePhysics(true);
        WeaponMesh->SetEnableGravity(true);
        WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        break;
    }
}

void AWeapons::dropped()
{
    SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
    WeaponMesh->DetachFromComponent(DetachmentRules);
	SetOwner(nullptr);

}

void AWeapons::OnRep_WeaponState()
{
    switch (WeaponState)
    {
    case EWeaponState::EWS_Equipped:
        ShowPickupWidget(false);
        WeaponMesh->SetSimulatePhysics(false);
        WeaponMesh->SetEnableGravity(false);
        WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        break;
    case EWeaponState::EWS_Dropped:
        WeaponMesh->SetSimulatePhysics(true);
        WeaponMesh->SetEnableGravity(true);
        WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        break;
    }
}

void AWeapons::ShowPickupWidget(bool bShowWidget)
{
    if (PickupWidget)
    {
        PickupWidget->SetVisibility(bShowWidget);
    }
}

void AWeapons::Fire(const FVector& HitTarget)
{
    if (FireAnimation)
    {
        WeaponMesh->PlayAnimation(FireAnimation, false);
    }
    if (CasingClass)
    {
        const USkeletalMeshSocket* AmmoEjectSocket = GetWeaponMesh()->GetSocketByName(FName("AmmoEject"));
        if (AmmoEjectSocket)
        {
            FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(GetWeaponMesh());
           
            FActorSpawnParameters SpawnParams;
            UWorld* World = GetWorld();
            if (World)
            {
                World->SpawnActor<ACasing>(
                    CasingClass,
                    SocketTransform.GetLocation(),
                    SocketTransform.GetRotation().Rotator(), 
                    SpawnParams
                );
            }
        }
    }
}
   