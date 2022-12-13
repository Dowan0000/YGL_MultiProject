// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacter.h"
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
	}

	/*FString RotationMesagge = FString::Printf(TEXT("Direction : %f"), Direction);
	GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Red, RotationMesagge);*/

}