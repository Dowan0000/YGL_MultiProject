// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotGun.h"
#include "MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"

AShotGun::AShotGun()
{
	ItemType = EItemType::EIT_Launcher;
	HandSocket = FName("Launcher_Socket");
}

void AShotGun::PressShoot_Implementation()
{
	if (bIsShoot) return;

	UE_LOG(LogTemp, Warning, TEXT("PressShoot"));

	const USkeletalMeshSocket* MeshSocketName = Mesh->GetSocketByName(MuzzleSocket);
	if (MeshSocketName)
	{
		FTransform SocketTransform = MeshSocketName->GetSocketTransform(Mesh);

		TArray<FHitResult> Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(GetOwner());

		UE_LOG(LogTemp, Warning, TEXT("LineTrace"));
		bool bResult = GetWorld()->SweepMultiByChannel(Hit, 
			SocketTransform.GetLocation(), 
			SocketTransform.GetLocation() + Character->GetActorForwardVector() * 1000.f,
			FQuat::Identity, 
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(300.f),
			Params);

		ShootEffectSound();

		bIsShoot = true;
		GetWorldTimerManager().SetTimer(ShootTimer, this,
			&AShotGun::CanShootTimer, 0.5f);

		DrawDebugLine(GetWorld(), SocketTransform.GetLocation(),
			SocketTransform.GetLocation() + Character->GetActorForwardVector() * 500.f,
			FColor::Red, false, 10.f);

		if (bResult)
		{
			UE_LOG(LogTemp, Warning, TEXT("Result"));

			for (FHitResult& Hitted : Hit)
			{
				FDamageEvent DamageEvent;
				Hitted.Actor->TakeDamage(Damage, DamageEvent,
					Character->GetController(), this);

			}
		}
	}

	CameraShake();
}

void AShotGun::CanShootTimer()
{
	bIsShoot = false;
}

void AShotGun::CameraShake_Implementation()
{
}