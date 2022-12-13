// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponInterface.h"
#include "ItemType.h"
#include "ItemState.h"
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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PressGetItem();
	virtual void PressGetItem_Implementation() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* Box;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class AMainCharacter* Character;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* ShootMontage;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Combat")
	class UParticleSystem* ShootEffect;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Item")
	EItemType ItemType;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	EItemState ItemState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName SocketName;

	// 인벤토리 아이템 배경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconBackground;

	// 무기 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconWeapon;

public:
	FORCEINLINE EItemType GetItemType() const { return ItemType; }
	
	void SetItemType(EItemType NewItemType) { ItemType = NewItemType; }

	void SetItemState(EItemState NewItemState);

};