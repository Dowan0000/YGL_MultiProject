// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "MainCharacter.h"
#include "MainHUD.h"

AMainPlayerController::AMainPlayerController() :
	GameStartTime(600.f), CurServerTime(0.f),
	CanStart(false)
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
		CurRestTime = GameStartTime - CurTime;
	}
	else
	{
		CurRestTime = GameStartTime - CurServerTime;
	}

	AMainHUD* HUD = Cast<AMainHUD>(GetHUD());
	if (HUD)
	{
		HUD->DrawHUDTime(CurRestTime);
	}

	if (0 <= CurRestTime)
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