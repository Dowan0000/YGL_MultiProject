// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "MainCharacter.h"
#include "MainHUD.h"

AMainPlayerController::AMainPlayerController() :
	GameStartTime(65.0f), CurServerTime(0.f),
	CanStart(false), min(5.f), ab(0)
{

}

void AMainPlayerController::Tick(float DeltaTimes)
{
	Super::Tick(DeltaTimes);

	SetHUDTime();

	if (!HasAuthority())
	{
		ServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

void AMainPlayerController::SetHUDTime()
{
	float CurTime;
	int CurRestTime;

	if (HasAuthority())
	{
		CurTime = GetWorld()->GetTimeSeconds();
		CurRestTime = GameStartTime + 60 * ab - CurTime;
	}
	else
	{
		CurRestTime = GameStartTime + 60 * ab - CurServerTime;
	}

	if (CurRestTime <= 0)
	{
		ab++;
		min--;
	}

	AMainHUD* HUD = Cast<AMainHUD>(GetHUD());
	if (HUD)
	{
		HUD->DrawHUDTime(CurRestTime, min);
	}

	if (CurRestTime < 60)
		CanStart = true;
}

void AMainPlayerController::ServerTime_Implementation(float TimeFromClient)
{
	float ServerTime = GetWorld()->GetTimeSeconds();
	ClientTime(TimeFromClient, ServerTime);
}

void AMainPlayerController::ClientTime_Implementation(float TimeFromServer, float ServerTime)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeFromServer;
	CurServerTime = ServerTime + RoundTripTime / 2.f;
}

void AMainPlayerController::IsDead_Implementation(APawn* DeadPawn)
{
}