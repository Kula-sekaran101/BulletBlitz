// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ACasing::ACasing()
{

	PrimaryActorTick.bCanEverTick = true;

	

	CasingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CasingBox"));
	SetRootComponent(CasingBox);
	


	CasingBox->SetSimulatePhysics(true);
	CasingBox->SetEnableGravity(true);
	CasingBox->SetNotifyRigidBodyCollision(true);

	

	CasingBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CasingBox->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	CasingBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CasingBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	ShellEjectionImpulse = 1500.0f; 

	
}


void ACasing::BeginPlay()
{
	Super::BeginPlay();

	CasingBox->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	CasingBox->AddImpulse(GetActorForwardVector()*ShellEjectionImpulse);
	
	
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Casing Hit!"));
	}

	if (CasingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CasingSound, GetActorLocation());
	}		

	Destroy();
}


