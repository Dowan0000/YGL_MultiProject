// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacter.h"
#include "WeaponBase.h"
#include "Kismet/KismetMathLibrary.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AMainCharacter>(TryGetPawnOwner());

}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Character == nullptr)
	{
		Character = Cast<AMainCharacter>(TryGetPawnOwner());
	}
	if (Character)
	{
		Speed = Character->GetVelocity().Size();
		IsInAir = Character->GetMovementComponent()->IsFalling();

		FRotator AimRotation = Character->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
		Direction = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
	
		if (Character->GetEquipWeapon())
		{
			ItemType = Character->GetEquipWeapon()->GetItemType();
		}
		
		IsCrouch = Character->bIsCrouched;

		// ³«ÇÏ µ¥¹ÌÁö
		if (IsInAir)
		{
			ZVelocity = Character->GetVelocity().Size();
			UE_LOG(LogTemp, Warning, TEXT("%f"), ZVelocity);
		}
		else
		{
			if (ZVelocity > 1300.f)
			{
				FDamageEvent Damage;
				Character->TakeDamage(ZVelocity * 0.02f, Damage, 
					GetWorld()->GetFirstPlayerController(), Character);
				ZVelocity = 0.f;
			}
		}

	}

	/*FString RotationMesagge = FString::Printf(TEXT("Direction : %f"), Direction);
	GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Red, RotationMesagge);*/

}

void UMainAnimInstance::IsDead()
{
	bIsDead = true;
}
