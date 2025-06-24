#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletBlitz/Types/TurningInPlace.h"
#include "BulletBlitz/Interfaces/InteractWithCrosshairsInterface.h"
#include "BulletBlitzCharacter.generated.h"

UCLASS()
class BULLETBLITZ_API ABulletBlitzCharacter : public ACharacter , public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABulletBlitzCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void Jump() override;
	void PlayFireMontage(bool bAiming);


	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHit();

protected:
	virtual void BeginPlay() override;

	// Input functions
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void PlayHitReactMontage();


	// Aim offset + turning
	void AimOffset(float DeltaTime);
	void TurnInPlace(float DeltaTime);

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	/** Widget to display overhead info */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	/** Weapon the player is overlapping */
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapons* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapons* LastWeapon);

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	/** Server RPC for equip */
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	/** Aim offset values */
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	/** Turning logic */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ETurningInPlace TurningInPlace;

	/** Fire animation montage */
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* HitReactMontage;

	

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnyWhere)
	float CameraThreshold = 200.f;


	//Player Health

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health ,VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health();

	class ABulletBlitzPlayerController* BulletBlitzPlayerController;

public:
	void SetOverlappingWeapon(AWeapons* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	AWeapons* GetEquippedWeapon();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
