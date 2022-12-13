// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ECrossHair.h"
#include "MainHUD.generated.h"

/**
 * 
 */
UCLASS()
class YGL_MULTIPROJECT_API AMainHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;

protected:
	virtual void BeginPlay() override;

	void CheckWeaponState();

	void SetCrossHairState(ECrossHair NewCrossHair);

	void DrawCrossHair(UTexture2D* Texture, float XAxis, float YAxis);

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AMainCharacter* Character;

		// Crosshair
	UPROPERTY(EditAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	class UTexture2D* CrossHairCenter;
	
	UPROPERTY(EditAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrossHairTop;

	UPROPERTY(EditAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrossHairBottom;

	UPROPERTY(EditAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrossHairLeft;

	UPROPERTY(EditAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrossHairRight;

	UPROPERTY(VisibleAnywhere, Category = "Crosshair")
	ECrossHair CrossHairState;

	float CrosshairInAir;
	float CrosshairIsShoot;
	float CrosshairCrouch = 0.f;

public:
	UFUNCTION(BlueprintNativeEvent)
	void SetHealth(float Health, float MaxHealth);

};
