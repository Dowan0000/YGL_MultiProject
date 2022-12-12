#pragma once

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Dropped UMETA(DisplayName = "Dropped"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_NonEquipped UMETA(DisplayName = "NonEquipped"),

};