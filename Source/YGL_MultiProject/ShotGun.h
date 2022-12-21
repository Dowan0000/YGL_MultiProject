// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "ShotGun.generated.h"

/**
 * 
 */
UCLASS()
class YGL_MULTIPROJECT_API AShotGun : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	AShotGun();

protected:
	virtual void PressShoot_Implementation() override;

	void CanShootTimer();
	
	UFUNCTION(BlueprintNativeEvent)
	void CameraShake();
};
