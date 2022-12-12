// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/BoxComponent.h"
#include "MainCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AWeaponBase::AWeaponBase() : 
	SocketName("Pistol_Socket")
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
	// LineTrace
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	ViewportSize.X /= 2;
	ViewportSize.Y /= 2;

	FVector WorldPosition;
	FVector WorldDirection;

	if (Character == nullptr) return;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController != Character->GetController()) return;
	
	UGameplayStatics::DeprojectScreenToWorld(
		PlayerController, ViewportSize,
		WorldPosition, WorldDirection);
	
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

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End,
		ECollisionChannel::ECC_Camera, Params);

	if (Hit.Actor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit.Actor.IsValid : %s"), *Hit.Actor->GetName());
		FDamageEvent Damage;
		Hit.Actor->TakeDamage(10.f, Damage,
			Character->GetController(), this);
	}

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.f);
}

void AWeaponBase::PressGetItem_Implementation()
{
	if (Character == nullptr) return;

	if (Character->GetEquipWeapon())
	{
		// 이미 가지고있는 아이템 예외처리...

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
