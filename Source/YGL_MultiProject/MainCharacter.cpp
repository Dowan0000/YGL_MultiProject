// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "WeaponBase.h"
#include "WeaponInterface.h"
#include "Net/UnrealNetwork.h"

AMainCharacter::AMainCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);

	SpringArm->bUsePawnControlRotation = true;

}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 0; i < 4; i++)
	{
		Inventory.Add(nullptr);
	}
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMainCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &AMainCharacter::LookRight);
	
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);

	// Shoot
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AMainCharacter::PressShoot);

	// GetItem
	PlayerInputComponent->BindAction(TEXT("GetItem"), EInputEvent::IE_Pressed, this, &AMainCharacter::PressGetItem);

	// SetWeapon
	PlayerInputComponent->BindAction(TEXT("SetPistol"), EInputEvent::IE_Pressed, this, &AMainCharacter::SetPistol);
	PlayerInputComponent->BindAction(TEXT("SetRifle"), EInputEvent::IE_Pressed, this, &AMainCharacter::SetRifle);
	PlayerInputComponent->BindAction(TEXT("SetSniper"), EInputEvent::IE_Pressed, this, &AMainCharacter::SetSniper);
	PlayerInputComponent->BindAction(TEXT("SetLauncher"), EInputEvent::IE_Pressed, this, &AMainCharacter::SetLauncher);

	// DropItem
	PlayerInputComponent->BindAction(TEXT("DropItem"), EInputEvent::IE_Pressed, this, &AMainCharacter::PressDropItem);

}

void AMainCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainCharacter, EquipWeapon);
	DOREPLIFETIME(AMainCharacter, OverlappingWeapon);
	DOREPLIFETIME(AMainCharacter, Health);
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= AppliedDamage;
	OnRep_Health();

	return AppliedDamage;
}

void AMainCharacter::OnRep_Health()
{

}

void AMainCharacter::MoveForward(float Value)
{
	if (Controller && Value)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (Controller && Value)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMainCharacter::LookRight(float Value)
{
	AddControllerYawInput(Value);
}

void AMainCharacter::PressShoot()
{
	ReqPressShoot();
}

void AMainCharacter::ReqPressShoot_Implementation()
{
	ResPressShoot();
}

void AMainCharacter::ResPressShoot_Implementation()
{
	if (EquipWeapon)
	{
		IWeaponInterface* Interface = Cast<IWeaponInterface>(EquipWeapon);
		if (Interface)
		{
			Interface->Execute_PressShoot(EquipWeapon);
		}
	}
}

void AMainCharacter::PressGetItem()
{
	ReqPressGetItem();
}

void AMainCharacter::ReqPressGetItem_Implementation()
{
	ResPressGetItem();
}

void AMainCharacter::ResPressGetItem_Implementation()
{
	if (OverlappingWeapon)
	{
		IWeaponInterface* Interface = Cast<IWeaponInterface>(OverlappingWeapon);
		if (Interface)
		{
			Interface->Execute_PressGetItem(OverlappingWeapon);
		}
	}

}

void AMainCharacter::SetPistol()
{
	if (EquipWeapon == nullptr) return;
	if (Inventory[0] == nullptr) return;

	if (Inventory[0]->GetItemType() == EItemType::EIT_Pistol)
	{
		EquipWeapon->SetItemState(EItemState::EIS_NonEquipped);
		Inventory[0]->SetItemState(EItemState::EIS_Equipped);
		EquipWeapon = Inventory[0];
	}
}

void AMainCharacter::SetRifle()
{
	if (EquipWeapon == nullptr) return;
	if (Inventory[1] == nullptr) return;

	if (Inventory[1]->GetItemType() == EItemType::EIT_Rifle)
	{
		EquipWeapon->SetItemState(EItemState::EIS_NonEquipped);
		Inventory[1]->SetItemState(EItemState::EIS_Equipped);
		EquipWeapon = Inventory[1];
	}
}

void AMainCharacter::SetSniper()
{
	if (EquipWeapon == nullptr) return;
	if (Inventory[2] == nullptr) return;

	if (Inventory[2]->GetItemType() == EItemType::EIT_Sniper)
	{
		EquipWeapon->SetItemState(EItemState::EIS_NonEquipped);
		Inventory[2]->SetItemState(EItemState::EIS_Equipped);
		EquipWeapon = Inventory[2];
	}
}

void AMainCharacter::SetLauncher()
{
	if (EquipWeapon == nullptr) return;
	if (Inventory[3] == nullptr) return;

	if (Inventory[3]->GetItemType() == EItemType::EIT_Launcher)
	{
		EquipWeapon->SetItemState(EItemState::EIS_NonEquipped);
		Inventory[3]->SetItemState(EItemState::EIS_Equipped);
		EquipWeapon = Inventory[3];
	}
}

void AMainCharacter::PressDropItem()
{
	ReqPressDropItem();
}

void AMainCharacter::ReqPressDropItem_Implementation()
{
	ResPressDropItem();
}

void AMainCharacter::ResPressDropItem_Implementation()
{
	if (Inventory[0] == nullptr) return;
	if (Inventory.Num() < 2) return;
	if (EquipWeapon == nullptr) return;
	if (EquipWeapon->GetItemType() == EItemType::EIT_Pistol) return;

	EquipWeapon->SetItemState(EItemState::EIS_Dropped);
	EquipWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	EquipWeapon->SetOwner(nullptr);
	SetDropInventory();
	Inventory[0]->SetItemState(EItemState::EIS_Equipped);
	EquipWeapon = Inventory[0];
	SetWeaponUI();
}

void AMainCharacter::SetDropInventory()
{
	if (EquipWeapon == nullptr) return;

	if (EquipWeapon->GetItemType() == EItemType::EIT_Pistol)
	{
		Inventory[0] = nullptr;
		HasPistol = false;
	}
	else if (EquipWeapon->GetItemType() == EItemType::EIT_Rifle)
	{
		Inventory[1] = nullptr;
		HasRifle = false;
	}
	else if (EquipWeapon->GetItemType() == EItemType::EIT_Sniper)
	{
		Inventory[2] = nullptr;
		HasSniper = false;
	}
	else if (EquipWeapon->GetItemType() == EItemType::EIT_Launcher)
	{
		Inventory[3] = nullptr;
		HasLauncher = false;
	}
}

void AMainCharacter::SetInventory()
{
	if (EquipWeapon == nullptr) return;

	if (EquipWeapon->GetItemType() == EItemType::EIT_Pistol)
	{
		Inventory[0] = EquipWeapon;
		HasPistol = true;
	}
	else if (EquipWeapon->GetItemType() == EItemType::EIT_Rifle)
	{
		Inventory[1] = EquipWeapon;
		HasRifle = true;
	}
	else if (EquipWeapon->GetItemType() == EItemType::EIT_Sniper)
	{
		Inventory[2] = EquipWeapon;
		HasSniper = true;
	}
	else if (EquipWeapon->GetItemType() == EItemType::EIT_Launcher)
	{
		Inventory[3] = EquipWeapon;
		HasLauncher = true;
	}
}

void AMainCharacter::SetWeaponUI_Implementation()
{

}