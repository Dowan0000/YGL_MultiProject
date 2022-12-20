// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotGun.h"
#include "MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"

AShotGun::AShotGun()
{
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
			SocketTransform.GetLocation(),
			FQuat::Identity, 
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(200.f),
			Params);


		ShootEffectSound();

		bIsShoot = true;
		GetWorldTimerManager().SetTimer(ShootTimer, this,
			&AShotGun::CanShootTimer, 0.5f);


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
}

void AShotGun::CanShootTimer()
{
	bIsShoot = false;
}