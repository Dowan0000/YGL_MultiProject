#pragma once

UENUM(BlueprintType)
enum class ECrossHair : uint8
{
	ECH_Pistol UMETA(DisplayName = "CH_Pistol"),
	ECH_Rifle UMETA(DisplayName = "CH_Rifle"),
	ECH_Sniper UMETA(DisplayName = "CH_Sniper"),
	ECH_Launcher UMETA(DisplayName = "CH_Launcher"),
};
