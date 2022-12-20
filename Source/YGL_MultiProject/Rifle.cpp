// Fill out your copyright notice in the Description page of Project Settings.


#include "Rifle.h"

ARifle::ARifle()
{
	ItemType = EItemType::EIT_Rifle;
	HandSocket = FName("Rifle_Socket");
}

void ARifle::PressShoot_Implementation()
{
	Super::PressShoot_Implementation();

	UE_LOG(LogTemp, Warning, TEXT("RifleShoot"));
	CameraShake();

	GetWorldTimerManager().SetTimer(ShootTimer, this,
		&ARifle::PressShootTimer, 0.2f);
}

void ARifle::CameraShake_Implementation()
{
}

void ARifle::PressShootTimer()
{
	if (bPressShoot)
		PressShoot_Implementation();

}