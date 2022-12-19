#pragma once

#include "Engine/DataTable.h"
#include "FST_Weapon.generated.h"

USTRUCT(BlueprintType)
struct FST_Weapon : public FTableRowBase
{
	GENERATED_BODY()

public:
	FST_Weapon()
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* ShootMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	class UParticleSystem* ShootEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	class USoundBase* ShootSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UParticleSystem* HitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
		int Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
		int MaxAmmo;
};