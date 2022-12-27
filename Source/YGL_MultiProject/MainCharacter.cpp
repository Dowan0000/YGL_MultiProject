// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "WeaponBase.h"
#include "WeaponInterface.h"
#include "Net/UnrealNetwork.h"
#include "MainHUD.h"
#include "Blueprint/UserWidget.h"
#include "MainAnimInstance.h"
#include "MainGameModeBase.h"
#include "MainPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AMainCharacter::AMainCharacter() : 
	HasPistol(false), HasRifle(false),
	HasSniper(false), HasLauncher(false),
	Health(100.f), MaxHealth(100.f),
	ZoomControlValue(1.f)

{
 	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);

	SpringArm->bUsePawnControlRotation = true;

	GetCharacterMovement()->JumpZVelocity = 600.f;
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 0; i < 4; i++)
		Inventory.Add(nullptr);
	
	void InitializeAmmoMap();

	if (BasicWeapon && HasAuthority())
	{
		BaseWeapon = GetWorld()->SpawnActor<AWeaponBase>(BasicWeapon, 
			GetActorTransform());
	}
	
	if (BaseWeapon)
		OnRep_BaseWeapon();

	BaseFOV = Camera->FieldOfView;

	if (CharacterMonTable)
	{
		CharacterData = CharacterMonTable->FindRow<FST_Character>(RowName, TEXT(""));
	}
}

void AMainCharacter::OnRep_BaseWeapon()
{
	BaseWeapon->SetCharacter(this);
	IWeaponInterface* Interface = Cast<IWeaponInterface>(BaseWeapon);
	if (Interface)
	{
		Interface->Execute_PressGetItem(BaseWeapon);
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
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMainCharacter::Turn);
	
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);

	// Shoot
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AMainCharacter::PressShoot);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Released, this, &AMainCharacter::ReleaseShoot);

	// GetItem
	PlayerInputComponent->BindAction(TEXT("GetItem"), EInputEvent::IE_Pressed, this, &AMainCharacter::PressGetItem);

	// SetWeapon
	PlayerInputComponent->BindAction(TEXT("SetPistol"), EInputEvent::IE_Pressed, this, &AMainCharacter::SetPistol);
	PlayerInputComponent->BindAction(TEXT("SetRifle"), EInputEvent::IE_Pressed, this, &AMainCharacter::SetRifle);
	PlayerInputComponent->BindAction(TEXT("SetSniper"), EInputEvent::IE_Pressed, this, &AMainCharacter::SetSniper);
	PlayerInputComponent->BindAction(TEXT("SetLauncher"), EInputEvent::IE_Pressed, this, &AMainCharacter::SetLauncher);

	// DropItem
	PlayerInputComponent->BindAction(TEXT("DropItem"), EInputEvent::IE_Pressed, this, &AMainCharacter::PressDropItem);

	// Zoom
	PlayerInputComponent->BindAction(TEXT("Zoom"), EInputEvent::IE_Pressed, this, &AMainCharacter::PressZoom);
	PlayerInputComponent->BindAction(TEXT("Zoom"), EInputEvent::IE_Released, this, &AMainCharacter::ReleasedZoom);

	// Crouch
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AMainCharacter::PressCrouch);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Released, this, &AMainCharacter::ReleaseCrouch);

}

void AMainCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainCharacter, EquipWeapon);
	DOREPLIFETIME(AMainCharacter, OverlappingWeapon);
	DOREPLIFETIME(AMainCharacter, Health);
	DOREPLIFETIME(AMainCharacter, BasicWeapon);
	DOREPLIFETIME(AMainCharacter, BaseWeapon);
	DOREPLIFETIME(AMainCharacter, Inventory);
	DOREPLIFETIME(AMainCharacter, RowName);
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= AppliedDamage;
	HitEffect();

	if (Health <= 0)
	{
		Health = 0;
		IsDead();
	}

	OnRep_Health();

	return AppliedDamage;
}

void AMainCharacter::OnRep_Health()
{
	if(HUD == nullptr)
		HUD = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	
	if (HUD)
	{
		HUD->SetHealth();
	}
}

void AMainCharacter::IsDead()
{
	ReqIsDead();
}

void AMainCharacter::ReqIsDead_Implementation()
{
	ResIsDead();
}

void AMainCharacter::ResIsDead_Implementation()
{
	UMainAnimInstance* Anim = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance());
	if (Anim)
	{
		Anim->IsDead();
	}

	AMainPlayerController* MainPC = Cast<AMainPlayerController>(GetController());
	if (MainPC)
	{
		MainPC->IsDead(this);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	EquipWeapon->SetItemState(EItemState::EIS_Dropped);
}

void AMainCharacter::HitEffect()
{
	ReqHitEffect();
}

void AMainCharacter::ReqHitEffect_Implementation()
{
	ResHitEffect();
}

void AMainCharacter::ResHitEffect_Implementation()
{
	if (HittedEffect && CharacterData->AttackedMontage)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
			HittedEffect,
			GetActorTransform());

		PlayAnimMontage(CharacterData->AttackedMontage, 1.f);
	}
	
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
	AddControllerPitchInput(Value * ZoomControlValue);
}

void AMainCharacter::Turn(float Value)
{
	AddControllerYawInput(Value * ZoomControlValue);
}

void AMainCharacter::PressShoot()
{
	if (EquipWeapon)
	{
		EquipWeapon->SetPressShoot(true);
	}

	ReqPressShoot();
}

void AMainCharacter::ReleaseShoot()
{
	if (EquipWeapon)
	{
		EquipWeapon->SetPressShoot(false);
	}
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

		/*FVector startPos = tpsCamComp->GetComponentLocation();
		FVector endPos = tpsCamComp->GetComponentLocation() + (tpsCamComp->GetForwardVector() * 5000);
		FHitResult hitInfo;
		FCollisionQueryParams params;
		params.AddIgnoredActor(me);
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);

		if (bHit)
		{
			FTransform bullletTrans;
			bullletTrans.SetLocation(hitInfo.ImpactPoint);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectfactory, bullletTrans);

			auto hitComp = hitInfo.GetComponent();
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 500000;
				hitComp->AddForce(force);
			}

			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enemy)
			{
				auto enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamageProcess();
			}*/

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

	ReqChangeItem(0);

	if (CharacterMonTable)
	{
		if (CharacterData)
		{
			if (CharacterData->PistolEquipMontage == nullptr) return;
			EquipMontage(CharacterData->PistolEquipMontage);
		}
	}
}

void AMainCharacter::SetRifle()
{
	if (EquipWeapon == nullptr) return;
	if (Inventory[1] == nullptr) return;

	ReqChangeItem(1);
	
	if (CharacterMonTable)
	{
		if (CharacterData)
		{
			if (CharacterData->RifleEquipMontage == nullptr) return;
			EquipMontage(CharacterData->RifleEquipMontage);
		}
	}
}

void AMainCharacter::SetSniper()
{
	if (EquipWeapon == nullptr) return;
	if (Inventory[2] == nullptr) return;

	ReqChangeItem(2);

	if (CharacterMonTable)
	{
		if (CharacterData)
		{
			if (CharacterData->SniperEquipMontage == nullptr) return;
			EquipMontage(CharacterData->SniperEquipMontage);
		}
	}
}

void AMainCharacter::SetLauncher()
{
	if (EquipWeapon == nullptr) return;
	if (Inventory[3] == nullptr) return;

	ReqChangeItem(3);

	if (CharacterMonTable)
	{
		if (CharacterData)
		{
			if (CharacterData->ShotGunEquipMontage == nullptr) return;
			EquipMontage(CharacterData->ShotGunEquipMontage);
		}
	}
}

void AMainCharacter::ReqChangeItem_Implementation(int ItemNum)
{
	ResChangeItem(ItemNum);
}

void AMainCharacter::ResChangeItem_Implementation(int ItemNum)
{
	EquipWeapon->SetItemState(EItemState::EIS_NonEquipped);
	Inventory[ItemNum]->SetItemState(EItemState::EIS_Equipped);
	EquipWeapon = Inventory[ItemNum];

	SetWeaponUI();
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

	if (CharacterMonTable)
	{
		if (CharacterData)
		{
			if (CharacterData->PistolEquipMontage == nullptr) return;
			EquipMontage(CharacterData->PistolEquipMontage);
		}
	}
}

void AMainCharacter::PressZoom()
{
	if (EquipWeapon && EquipWeapon->GetItemType() == EItemType::EIT_Sniper)
	{
		if (ZoomWidgetClass)
		{
			ZoomWidget = CreateWidget<UUserWidget>(GetWorld(), ZoomWidgetClass);
			if (ZoomWidget)
			{
				ZoomWidget->AddToViewport();

				Camera->FieldOfView = BaseFOV - 70.f;
				ZoomControlValue = 0.5f; // zoom 마우스 감도

			}
		}
	}
}

void AMainCharacter::ReleasedZoom()
{
	if (EquipWeapon && EquipWeapon->GetItemType() == EItemType::EIT_Sniper)
	{
		if (ZoomWidgetClass && ZoomWidget)
		{
			ZoomWidget->RemoveFromParent();

			Camera->FieldOfView = BaseFOV;
			ZoomControlValue = 1.f;

		}
	}
}

void AMainCharacter::PressCrouch()
{
	ReqCrouch(true);
}

void AMainCharacter::ReleaseCrouch()
{
	ReqCrouch(false);
}

void AMainCharacter::ReqCrouch_Implementation(bool bIsCrouch)
{
	ResCrouch(bIsCrouch);
}

void AMainCharacter::ResCrouch_Implementation(bool bIsCrouch)
{
	if (bIsCrouch) Crouch();
	else UnCrouch();
}

void AMainCharacter::EquipMontage(UAnimMontage* EquipMontage)
{
	ReqEquipMontage(EquipMontage);
}

void AMainCharacter::ReqEquipMontage_Implementation(UAnimMontage* EquipMontage)
{
	ResEquipMontage(EquipMontage);
}

void AMainCharacter::ResEquipMontage_Implementation(UAnimMontage* EquipMontage)
{
	PlayAnimMontage(EquipMontage, 1.f);
}


void AMainCharacter::SetInventory()
{
	if (EquipWeapon == nullptr) return;

	if (EquipWeapon->GetItemType() == EItemType::EIT_Pistol)
	{
		Inventory[0] = EquipWeapon;
		HasPistol = true;

		if (CharacterMonTable)
		{
			if (CharacterData)
			{
				if (CharacterData->PistolEquipMontage == nullptr) return;
				EquipMontage(CharacterData->PistolEquipMontage);
			}
		}
	}
	else if (EquipWeapon->GetItemType() == EItemType::EIT_Rifle)
	{
		Inventory[1] = EquipWeapon;
		HasRifle = true;

		if (CharacterMonTable)
		{
			if (CharacterData)
			{
				if (CharacterData->RifleEquipMontage == nullptr) return;
				EquipMontage(CharacterData->RifleEquipMontage);
			}
		}
	}
	else if (EquipWeapon->GetItemType() == EItemType::EIT_Sniper)
	{
		Inventory[2] = EquipWeapon;
		HasSniper = true;

		if (CharacterMonTable)
		{
			if (CharacterData)
			{
				if (CharacterData->SniperEquipMontage == nullptr) return;
				EquipMontage(CharacterData->SniperEquipMontage);
			}
		}
	}
	else if (EquipWeapon->GetItemType() == EItemType::EIT_Launcher)
	{
		Inventory[3] = EquipWeapon;
		HasLauncher = true;

		if (CharacterMonTable)
		{
			if (CharacterData)
			{
				if (CharacterData->ShotGunEquipMontage == nullptr) return;
				EquipMontage(CharacterData->ShotGunEquipMontage);
			}
		}
	}
}

void AMainCharacter::SetWeaponUI()
{
	if (HUD == nullptr)
		HUD = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	if (HUD)
	{
		HUD->SetWeaponUI();
	}
}
