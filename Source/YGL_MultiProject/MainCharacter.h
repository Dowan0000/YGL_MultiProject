// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

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

	void PressGetItem();
	UFUNCTION(Server, Reliable)
	void ReqPressGetItem();
	UFUNCTION(NetMulticast, Reliable)
	void ResPressGetItem();

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

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class AWeaponBase* EquipWeapon;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	AWeaponBase* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TArray<AWeaponBase*> Inventory;

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

public:	
	FORCEINLINE AWeaponBase* GetEquipWeapon() const { return EquipWeapon; }
	FORCEINLINE void SetEquipWeapon(AWeaponBase* NewWeapon) { EquipWeapon = NewWeapon; }

	FORCEINLINE void SetOverlappingWeapon(AWeaponBase* NewWeapon) { OverlappingWeapon = NewWeapon; }

	void SetInventory();
	
	UFUNCTION(BlueprintNativeEvent)
	void SetWeaponUI();

};
