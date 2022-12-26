// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Rifle.generated.h"

/**
 * 
 */
UCLASS()
class YGL_MULTIPROJECT_API ARifle : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	ARifle();

protected:
	virtual void PressShoot_Implementation() override;

	UFUNCTION(BlueprintNativeEvent)
	void PressShootTimer();

	UFUNCTION(BlueprintNativeEvent)
	void CameraShake();

	FTimerHandle ShootTimer;
};
