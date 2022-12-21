// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/BoxComponent.h"
#include "MainCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AWeaponBase::AWeaponBase() : 
	HandSocket("Pistol_Socket"), MuzzleSocket("MuzzleFlash"),
	RowName("1"), Damage(10.f),
	Ammo(0)
{
 	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	
	SetRootComponent(Mesh);
	Box->SetupAttachment(RootComponent);

	Mesh->SetSimulatePhysics(true);

	Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);

	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::BoxBeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::BoxEndOverlap);

	if (WeaponTable)
	{
		WeaponData = WeaponTable->FindRow<FST_Weapon>(RowName, TEXT(""));
		UE_LOG(LogTemp, Warning, TEXT("WeaponBegin"));
	}
		
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBase, ItemType);
	DOREPLIFETIME(AWeaponBase, ItemState);
	DOREPLIFETIME(AWeaponBase, RowName);
}

void AWeaponBase::BoxBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Character = Cast<AMainCharacter>(OtherActor);
	if (Character)
	{
		Character->SetOverlappingWeapon(this);
	}
}

void AWeaponBase::BoxEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Character)
	{
		Character->SetOverlappingWeapon(nullptr);
	}
}

void AWeaponBase::PressShoot_Implementation()
{
	// LineTrace
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	ViewportSize.X /= 2;
	ViewportSize.Y /= 2;

	FVector WorldPosition;
	FVector WorldDirection;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController != Character->GetController()) return;

	if (Character)
	{
		UGameplayStatics::DeprojectScreenToWorld(
			PlayerController, ViewportSize,
			WorldPosition, WorldDirection);
	}

	FVector Start = WorldPosition;
	FVector End = WorldPosition + WorldDirection * 50'000.f;

	bIsShoot = true;
	GetWorldTimerManager().SetTimer(ShootTimer, this,
		&AWeaponBase::CanShootTimer, 0.2f);

	ReqShoot(Start, End);
}

void AWeaponBase::ReqShoot_Implementation(FVector Start, FVector End)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	UE_LOG(LogTemp, Warning, TEXT("LineTrace"));
	bool bResult = GetWorld()->LineTraceSingleByChannel(Hit, Start, End,
		ECollisionChannel::ECC_Camera, Params);

	ShootEffectSound();

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.f);

	if (bResult)
	{
		HitEffectSound(Hit.Location);

		if (Hit.Actor.IsValid())
		{
			// °°ÀºÆÀ °ø°Ýx
			AMainCharacter* HitActor = Cast<AMainCharacter>(Hit.Actor);
			if (HitActor == nullptr) return;
			
			if (Character->GetTeam() == HitActor->GetTeam()) return;
				

			FDamageEvent DamageEvent;
			if (Hit.BoneName.ToString() == "head")
			{
				Hit.Actor->TakeDamage(Damage * 2.f, DamageEvent,
					Character->GetController(), this);
			}
			else if (Hit.BoneName.ToString() == "pelvis" ||
				Hit.BoneName.ToString() == "spine_01" ||
				Hit.BoneName.ToString() == "spine_02" ||
				Hit.BoneName.ToString() == "spine_03")
			{
				Hit.Actor->TakeDamage(Damage * 1.5f, DamageEvent,
					Character->GetController(), this);
			}
			else
			{
				Hit.Actor->TakeDamage(Damage, DamageEvent,
					Character->GetController(), this);
			}

			UE_LOG(LogTemp, Warning, TEXT("Hit.BoneName : %s"), *Hit.BoneName.ToString());
		}
	}
}

void AWeaponBase::ShootEffectSound_Implementation()
{
	if (Character == nullptr) return;

	if (WeaponData->ShootMontage)
	{
		Character->PlayAnimMontage(WeaponData->ShootMontage);
	}

	const USkeletalMeshSocket* MeshSocketName = Mesh->GetSocketByName(MuzzleSocket);
	if (MeshSocketName)
	{
		FTransform SocketTransform = MeshSocketName->GetSocketTransform(Mesh);

		if (WeaponData->ShootEffect && WeaponData->ShootSound)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
				WeaponData->ShootEffect, SocketTransform);
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(),
				WeaponData->ShootSound, SocketTransform.GetLocation());
		}
	}
}

void AWeaponBase::HitEffectSound_Implementation(FVector Location)
{
	if (WeaponData->HitEffect && WeaponData->HitSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("HitEffect,Sound"));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
			WeaponData->HitEffect, Location);
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(),
			WeaponData->HitSound, Location);
	}
}

void AWeaponBase::CanShootTimer()
{
	bIsShoot = false;
}

void AWeaponBase::PressGetItem_Implementation()
{
	if (Character == nullptr) return;

	if (Character->GetEquipWeapon())
	{
		Character->GetEquipWeapon()->SetItemState(EItemState::EIS_NonEquipped);

		SetItemState(EItemState::EIS_Equipped);

		const USkeletalMeshSocket* MeshSocketName = Character->GetMesh()->GetSocketByName(HandSocket);
		if (MeshSocketName)
		{
			MeshSocketName->AttachActor(this, Character->GetMesh());
		}

		SetOwner(Character);
		Character->SetEquipWeapon(this);
		Character->SetInventory();
		Character->SetWeaponUI();
	}
	else
	{
		SetItemState(EItemState::EIS_Equipped);

		const USkeletalMeshSocket* MeshSocketName = Character->GetMesh()->GetSocketByName(HandSocket);
		if (MeshSocketName)
		{
			MeshSocketName->AttachActor(this, Character->GetMesh());
		}

		SetOwner(Character);
		Character->SetEquipWeapon(this);
		Character->SetInventory();
		Character->SetWeaponUI();
	}
}

void AWeaponBase::SetItemState(EItemState NewItemState)
{
	switch (NewItemState)
	{
	case EItemState::EIS_Dropped:
		Mesh->SetSimulatePhysics(true);
		Mesh->SetVisibility(true);

		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

		Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		break;
	case EItemState::EIS_Equipped:
		Mesh->SetSimulatePhysics(false);
		Mesh->SetVisibility(true);

		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EItemState::EIS_NonEquipped:
		Mesh->SetSimulatePhysics(false);
		Mesh->SetVisibility(false);

		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	}
}
