// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponInterface.h"
#include "ItemType.h"
#include "ItemState.h"
#include "FST_Weapon.h"
#include "WeaponBase.generated.h"

UCLASS()
class YGL_MULTIPROJECT_API AWeaponBase : public AActor, public IWeaponInterface
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();
	
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent)
	void BoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PressShoot();
	virtual void PressShoot_Implementation() override;
	UFUNCTION(Server, Reliable)
	void ReqShoot(FVector Start, FVector End);

	UFUNCTION(NetMulticast, Reliable)
	void ShootEffectSound();
	UFUNCTION(NetMulticast, Reliable)
	void HitEffectSound(FVector Location);

	// crosshair
	void CanShootTimer();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PressGetItem();
	virtual void PressGetItem_Implementation() override;

private:
	// 현재 무기 ammo count 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* Box;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class AMainCharacter* Character;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Item")
	EItemType ItemType;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	EItemState ItemState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName HandSocket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName MuzzleSocket;

	// 인벤토리 아이템 배경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconBackground;

	// 무기 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDataTable* WeaponTable;

	FST_Weapon* WeaponData;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FName RowName;

	// crosshair
	bool bIsShoot;

	FTimerHandle ShootTimer;

	UPROPERTY(VisibleAnywhere, Category = "Shoot")
		bool bPressShoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float Damage;

	// ShootMontage
	

public:
	FORCEINLINE EItemType GetItemType() const { return ItemType; }
	
	void SetItemType(EItemType NewItemType) { ItemType = NewItemType; }

	void SetItemState(EItemState NewItemState);

	FORCEINLINE bool GetbIsShoot() const { return bIsShoot; }

	FORCEINLINE void SetPressShoot(bool NewPress) { bPressShoot = NewPress; }

	FORCEINLINE int32 GetAmmo() const { return Ammo; }

	void SetCharacter(AMainCharacter* NewCharacter) { Character = NewCharacter; }


};