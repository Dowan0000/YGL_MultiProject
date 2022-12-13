// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponInterface.h"
#include "ItemType.h"
#include "ItemState.h"
#include "Engine/DataTable.h"
#include "WeaponBase.generated.h"

USTRUCT(BlueprintType)
struct FST_Weapon : public FTableRowBase
{
	GENERATED_BODY()

public:
	FST_Weapon()
	{}

};

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

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Item")
	EItemType ItemType;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	EItemState ItemState;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* ShootMontage;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Combat")
	class UParticleSystem* ShootEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName SocketName;

	

	

public:
	FORCEINLINE EItemType GetItemType() const { return ItemType; }
	
	void SetItemType(EItemType NewItemType) { ItemType = NewItemType; }

	void SetItemState(EItemState NewItemState);

};
