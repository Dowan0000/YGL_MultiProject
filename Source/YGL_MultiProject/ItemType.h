#pragma once

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_None UMETA(DisplayName = "None"),
	EIT_Pistol UMETA(DisplayName = "Pistol"),
	EIT_Rifle UMETA(DisplayName = "Rifle"),
	EIT_Sniper UMETA(DisplayName = "Sniper"),
	EIT_Launcher UMETA(DisplayName = "Launcher"),

};