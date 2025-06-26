#include "BulletBlitzCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "BulletBlitz/Weapons/Weapons.h"
#include "BulletBlitz/BulletBlitzComponent/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyAnimInstance.h"
#include "BulletBlitz/PlayerController/BulletBlitzPlayerController.h"


ABulletBlitzCharacter::ABulletBlitzCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	// Limit the pitch (rotate camera up/down)
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;

	// Limit pitch here
	CameraBoom->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 300.0f;

	CameraBoom->bDoCollisionTest = true;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}

void ABulletBlitzCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABulletBlitzCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABulletBlitzCharacter, Health);
}

void ABulletBlitzCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABulletBlitzCharacter::ReceiveDamage);
	}

	
}

void ABulletBlitzCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimOffset(DeltaTime);
	HideCameraIfCharacterClose();
}

void ABulletBlitzCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Combat->PrimaryComponentTick.bCanEverTick = true;
	if (Combat)
	{
		Combat->Character = this;
	}
}

void ABulletBlitzCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABulletBlitzCharacter::Jump);
	PlayerInputComponent->BindAxis("MoveForward", this, &ABulletBlitzCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABulletBlitzCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABulletBlitzCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABulletBlitzCharacter::LookUp);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABulletBlitzCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABulletBlitzCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABulletBlitzCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABulletBlitzCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABulletBlitzCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABulletBlitzCharacter::FireButtonReleased);
}

void ABulletBlitzCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void ABulletBlitzCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void ABulletBlitzCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABulletBlitzCharacter::LookUp(float Value)
{
	//AddControllerPitchInput(Value);
	if (Controller && Value != 0.f)
	{
		float Sensitivity = 2.0f; // Increase this to make it more sensitive (default ~1.0)

		FRotator CurrentRotation = Controller->GetControlRotation();

		float NormalizedPitch = FRotator::NormalizeAxis(CurrentRotation.Pitch);

		float MinPitch = -45.f;
		float MaxPitch = 45.f;

		float NewPitch = FMath::Clamp(NormalizedPitch - Value * Sensitivity, MinPitch, MaxPitch);

		Controller->SetControlRotation(FRotator(NewPitch, CurrentRotation.Yaw, CurrentRotation.Roll));
	}
}

void ABulletBlitzCharacter::EquipButtonPressed()
{
	if (Combat)
	{

		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void ABulletBlitzCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ABulletBlitzCharacter::CrouchButtonPressed()
{
	if (bIsCrouched) UnCrouch();
	else Crouch();
}

void ABulletBlitzCharacter::AimButtonPressed()
{
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABulletBlitzCharacter::AimButtonReleased()
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ABulletBlitzCharacter::FireButtonPressed()
{
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ABulletBlitzCharacter::FireButtonReleased()
{
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void ABulletBlitzCharacter::Jump()
{
	if (bIsCrouched) UnCrouch();
	else Super::Jump();
}



void ABulletBlitzCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
		if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
		{
			GetMesh()->SetVisibility(false);
			if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
			{
				Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
			}
		}
		else
		{
			GetMesh()->SetVisibility(true);
			if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
			{
				Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
			}
		}
}

void ABulletBlitzCharacter::OnRep_Health()
{
	PlayHitReactMontage();
	UpdateHUDHealth();
}


void ABulletBlitzCharacter::UpdateHUDHealth()
{

	if (BulletBlitzPlayerController == nullptr)
	{
		BulletBlitzPlayerController = Cast<ABulletBlitzPlayerController>(Controller);
	}

	if (BulletBlitzPlayerController)
	{
		BulletBlitzPlayerController->SetHUDHealth(Health, MaxHealth);
	}


}

void ABulletBlitzCharacter::SetOverlappingWeapon(AWeapons* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled() && OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
}

void ABulletBlitzCharacter::OnRep_OverlappingWeapon(AWeapons* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABulletBlitzCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABulletBlitzCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("From_Front");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABulletBlitzCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir)
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;

		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}

		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir)
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABulletBlitzCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 60.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -60.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;

		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ABulletBlitzCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	
	PlayHitReactMontage();
	UpdateHUDHealth();
}





bool ABulletBlitzCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABulletBlitzCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapons* ABulletBlitzCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector ABulletBlitzCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}
