// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"



void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || !IsValid(OtherActor))
    {
        Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
        return;
    }


    if (HasAuthority())
    {
        ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter && IsValid(OwnerCharacter))
        {
            AController* OwnerController = OwnerCharacter->Controller;
            if (OwnerController && IsValid(OwnerController))
            {
                UGameplayStatics::ApplyDamage(
                    OtherActor,
                    Damage,
                    OwnerController,
                    this,
                    UDamageType::StaticClass()
                );
            }
        }
    }

    
    Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

   
    Destroy();
}
