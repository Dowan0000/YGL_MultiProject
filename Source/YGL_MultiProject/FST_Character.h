#pragma once

#include "Engine/DataTable.h"
#include "FST_Character.generated.h"

USTRUCT(BlueprintType)
struct FST_Character : public FTableRowBase
{
	GENERATED_BODY()

public:
	FST_Character()
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
		class UAnimMontage* PistolShootMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* RifleShootMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* ShotGunShootMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* PistolEquipMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* RifleEquipMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
		UAnimMontage* SniperEquipMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* ShotGunEquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* AttackedMontage;

};