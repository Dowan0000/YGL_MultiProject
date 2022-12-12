// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/BoxComponent.h"
#include "MainCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"

AWeaponBase::AWeaponBase() : 
	SocketName("Pistol_Socket")
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
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBase, ShootMontage);
	DOREPLIFETIME(AWeaponBase, ShootEffect);
	DOREPLIFETIME(AWeaponBase, ItemType);
	DOREPLIFETIME(AWeaponBase, ItemState);
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

}

void AWeaponBase::PressGetItem_Implementation()
{
	if (Character == nullptr) return;

	if (Character->GetEquipWeapon())
	{
		Character->GetEquipWeapon()->SetItemState(EItemState::EIS_NonEquipped);

		SetItemState(EItemState::EIS_Equipped);

		const USkeletalMeshSocket* MeshSocketName = Character->GetMesh()->GetSocketByName(SocketName);
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

		const USkeletalMeshSocket* MeshSocketName = Character->GetMesh()->GetSocketByName(SocketName);
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
