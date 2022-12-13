// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"
#include "MainCharacter.h"
#include "WeaponBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void AMainHUD::DrawHUD()
{
	Super::DrawHUD();

	CheckWeaponState();
}

void AMainHUD::BeginPlay()
{
}

void AMainHUD::CheckWeaponState()
{
	if (Character == nullptr)
	{
		Character = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	}
	if (Character && Character->GetEquipWeapon())
	{
		if (Character->GetEquipWeapon()->GetItemType() == EItemType::EIT_Pistol)
		{
			SetCrossHairState(ECrossHair::ECH_Pistol);
		}
		if (Character->GetEquipWeapon()->GetItemType() == EItemType::EIT_Rifle)
		{
			SetCrossHairState(ECrossHair::ECH_Rifle);
		}
		
		if (Character->GetEquipWeapon()->GetItemType() == EItemType::EIT_Sniper)
		{
			SetCrossHairState(ECrossHair::ECH_Sniper);
		}
		if (Character->GetEquipWeapon()->GetItemType() == EItemType::EIT_Launcher)
		{
			SetCrossHairState(ECrossHair::ECH_Launcher);
		}
	}
}

void AMainHUD::SetCrossHairState(ECrossHair NewCrossHair)
{
	if (Character == nullptr)
	{
		Character = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	}
	if (Character == nullptr) return;

	FVector2D WalkSpeed(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
	FVector2D VelocityRange(0.f, 10.f);
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;
	float VelocitySize = Velocity.Size();

	float CrosshairVelocity = FMath::GetMappedRangeValueClamped(WalkSpeed, VelocityRange, VelocitySize) + 1;

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, FString::Printf(TEXT("%f"), CrosshairVelocity));

	// Crosshair When Jump
	if (Character->GetCharacterMovement()->IsFalling())
	{
		CrosshairInAir = FMath::FInterpTo(CrosshairInAir, 10.f, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 2.f);
	}
	else
	{
		CrosshairInAir = FMath::FInterpTo(CrosshairInAir, 0.f, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 20.f);
	}

	// Crosshair When Shoot
	if (Character->GetEquipWeapon()->GetbIsShoot())
	{
		CrosshairIsShoot = FMath::FInterpTo(CrosshairIsShoot, 10.f, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 10.f);

	}
	else
	{
		CrosshairIsShoot = FMath::FInterpTo(CrosshairIsShoot, 0.f, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 5.f);
		
	}
	
	// Crosshair When Crouch

	/*if (Character->GetbIsCrouch())
	{
		CrosshairCrouch = FMath::FInterpTo(CrosshairCrouch, 5.f, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 10.f);
	}
	else
	{
		CrosshairCrouch = FMath::FInterpTo(CrosshairCrouch, 0.f, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 10.f);
	}*/

	float DrawCrossHairXAxis;
	float DrawCrossHairYAxis;

	switch (NewCrossHair)
	{
	case ECrossHair::ECH_Pistol:
		DrawCrossHairXAxis = 10.f + CrosshairVelocity + CrosshairInAir + CrosshairIsShoot - CrosshairCrouch;
		DrawCrossHairYAxis = 10.f + CrosshairVelocity + CrosshairInAir + CrosshairIsShoot - CrosshairCrouch;

		DrawCrossHair(CrossHairTop, 0.f, -DrawCrossHairYAxis);
		DrawCrossHair(CrossHairBottom, 0.f, DrawCrossHairYAxis);
		DrawCrossHair(CrossHairLeft, -DrawCrossHairXAxis, 0.f);
		DrawCrossHair(CrossHairRight, DrawCrossHairXAxis, 0.f);

		break;
	case ECrossHair::ECH_Rifle:
		DrawCrossHairXAxis = CrosshairVelocity + CrosshairInAir + CrosshairIsShoot - CrosshairCrouch;
		DrawCrossHairYAxis = CrosshairVelocity + CrosshairInAir + CrosshairIsShoot - CrosshairCrouch;

		DrawCrossHair(CrossHairTop, 0.f, -DrawCrossHairYAxis);
		DrawCrossHair(CrossHairBottom, 0.f, DrawCrossHairYAxis);
		DrawCrossHair(CrossHairLeft, -DrawCrossHairXAxis, 0.f);
		DrawCrossHair(CrossHairRight, DrawCrossHairXAxis, 0.f);

		break;
	case ECrossHair::ECH_Sniper:
		DrawCrossHair(CrossHairCenter, 0.f, 0.f);

		break;
	case ECrossHair::ECH_Launcher:
		DrawCrossHair(CrossHairCenter, 0.f, 0.f);

		break;
	}

}

void AMainHUD::DrawCrossHair(UTexture2D* Texture, float XAxis, float YAxis)
{
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	FVector2D ViewportCenter(ViewportSize.X / 2, ViewportSize.Y / 2);

	float TextureWidth = CrossHairTop->GetSizeX() / 1.2f;
	float TextureHeight = CrossHairTop->GetSizeY() / 1.2f;

	FVector2D TextureDrawPoint(ViewportCenter.X - TextureWidth / 2.f + XAxis,
		ViewportCenter.Y - TextureHeight / 2.f + YAxis);

	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y,
		TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f);
}