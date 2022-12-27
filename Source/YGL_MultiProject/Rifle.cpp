// Fill out your copyright notice in the Description page of Project Settings.


#include "Rifle.h"
#include "MainCharacter.h"

ARifle::ARifle()
{
	ItemType = EItemType::EIT_Rifle;
	HandSocket = FName("Rifle_Socket");
}

void ARifle::PressShoot_Implementation()
{
	Super::PressShoot_Implementation();

	UE_LOG(LogTemp, Warning, TEXT("RifleShoot1111111"));
	CameraShake();

	GetWorldTimerManager().SetTimer(ShootTimer, this,
		&ARifle::PressShootTimer, 0.15f);
}

void ARifle::CameraShake_Implementation()
{
}

void ARifle::PressShootTimer_Implementation()
{
	if (Character->Inventory[1] && 
		Character->GetEquipWeapon()->GetItemType() == EItemType::EIT_Rifle)
	{
		if (bPressShoot)
			PressShoot_Implementation();
	}
}