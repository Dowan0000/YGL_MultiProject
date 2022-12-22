// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Team.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_PistolAmmo UMETA(DisplayName = "PistolAmmo"),
	EAT_RifleAmmo UMETA(DisplayName = "RifleAmmo"),
	EAT_SniperAmmo UMETA(DisplayName = "SniperAmmo"),

	EAT_NAX UMETA(DisplayName = "DefaultMAX")
};


UCLASS()
class YGL_MULTIPROJECT_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	void PressShoot();
	UFUNCTION(Server, Reliable)
	void ReqPressShoot();
	UFUNCTION(NetMulticast, Reliable)
	void ResPressShoot();

	void ReleaseShoot();

	void PressGetItem();
	UFUNCTION(Server, Reliable)
	void ReqPressGetItem();
	UFUNCTION(NetMulticast, Reliable)
	void ResPressGetItem();

	UFUNCTION()
	void OnRep_BaseWeapon();

	void SetPistol();
	void SetRifle();
	void SetSniper();
	void SetLauncher();

	UFUNCTION(Server, Reliable)
	void ReqChangeItem(int ItemNum);
	UFUNCTION(NetMulticast, Reliable)
	void ResChangeItem(int ItemNum);

	void PressDropItem();
	UFUNCTION(Server, Reliable)
	void ReqPressDropItem();
	UFUNCTION(NetMulticast, Reliable)
	void ResPressDropItem();

	void SetDropInventory();

	UFUNCTION()
	void OnRep_Health();

	void IsDead();
	UFUNCTION(Server, Reliable)
	void ReqIsDead();
	UFUNCTION(NetMulticast, Reliable)
	void ResIsDead();

	void HitEffect();
	UFUNCTION(Server, Reliable)
		void ReqHitEffect();
	UFUNCTION(NetMulticast, Reliable)
		void ResHitEffect();

	void PressZoom();
	void ReleasedZoom();

	void PressCrouch();
	void ReleaseCrouch();
	UFUNCTION(Server, Reliable)
	void ReqCrouch(bool bIsCrouch);
	UFUNCTION(NetMulticast, Reliable)
	void ResCrouch(bool bIsCrouch);

	void InitializeAmmoMap();

	bool WeaponHasAmmo();


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class AWeaponBase* EquipWeapon;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	AWeaponBase* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
		bool HasPistol;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
		bool HasRifle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
		bool HasSniper;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
		bool HasLauncher;

	UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	// Zoom
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UUserWidget> ZoomWidgetClass;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UUserWidget* ZoomWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoom", meta = (AllowPrivateAccess = "true"))
		float ZoomControlValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	class AMainHUD* HUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Team", meta = (ExposeOnSpawn = "true"), meta = (AllowPrivateAccess = "true"))
	ETeam Team;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
		int32 StartingPistolAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
		int32 StartingRifleAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
		int32 StartingSniperAmmo;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeaponBase> BasicWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_BaseWeapon, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AWeaponBase* BaseWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* HittedEffect;

	FTimerHandle BaseWeaponTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoom", meta = (AllowPrivateAccess = "true"))
	float BaseFOV;

public:	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<AWeaponBase*> Inventory;

	FORCEINLINE AWeaponBase* GetEquipWeapon() const { return EquipWeapon; }
	FORCEINLINE void SetEquipWeapon(AWeaponBase* NewWeapon) { EquipWeapon = NewWeapon; }

	FORCEINLINE void SetOverlappingWeapon(AWeaponBase* NewWeapon) { OverlappingWeapon = NewWeapon; }

	void SetInventory();
	
	void SetWeaponUI();

	FORCEINLINE ETeam GetTeam() const { return Team; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
		class UAnimMontage* PistolShootMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
		class UAnimMontage* RifleShootMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
		class UAnimMontage* ShotGunShootMontage;

};
