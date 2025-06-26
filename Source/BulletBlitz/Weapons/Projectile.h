// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BULLETBLITZ_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	AProjectile();
	virtual void Tick(float DeltaTime) override; 

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) ;

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

public:	
	


private:

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BulletMesh;

	UPROPERTY(VisibleAnywhere, Category = "Projectile Properties")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	class UParticleSystem* Tracer;

	
	class UParticleSystemComponent* TracerComponent;

	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	class USoundCue* ImpactSound;

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastImpactEffects();

	
	


};


