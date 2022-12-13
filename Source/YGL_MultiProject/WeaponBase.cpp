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
	RowName("1")
{
 	PrimaryActorTick.bCanEverTick = true;

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

	/*DOREPLIFETIME(AWeaponBase, ShootMontage);
	DOREPLIFETIME(AWeaponBase, ShootEffect);*/
	DOREPLIFETIME(AWeaponBase, ItemType);
	DOREPLIFETIME(AWeaponBase, ItemState);
	DOREPLIFETIME(AWeaponBase, RowName);
}

void AWeaponBase::BoxBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Character == nullptr)
	{
		Character = Cast<AMainCharacter>(OtherActor);
	}
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

	ReqShoot(Start, End);
}

void AWeaponBase::ReqShoot_Implementation(FVector Start, FVector End)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	bool bResult = GetWorld()->LineTraceSingleByChannel(Hit, Start, End,
		ECollisionChannel::ECC_Camera, Params);

	ShootEffectSound();

	if (!bResult) return;

	if (Hit.Actor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit.Actor.IsValid : %s"), *Hit.Actor->GetName());
		FDamageEvent Damage;
		Hit.Actor->TakeDamage(10.f, Damage,
			Character->GetController(), this);
		
		HitEffectSound(Hit.Location);
	}

	bIsShoot = true;
	GetWorldTimerManager().SetTimer(ShootTimer, this,
		&AWeaponBase::CanShootTimer, 0.2f);

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.f);
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
