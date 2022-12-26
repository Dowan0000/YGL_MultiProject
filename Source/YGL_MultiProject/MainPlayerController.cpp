// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "MainCharacter.h"
#include "MainHUD.h"

AMainPlayerController::AMainPlayerController() :
	GameStartTime(10.0f), CurServerTime(0.f),
	min(5.f), ab(0), WaitTime(true)
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

	// 2초 어디감
	if (HasAuthority())
	{
		CurTime = GetWorld()->GetTimeSeconds();
		CurRestTime = GameStartTime + 61 * ab - CurTime;
	}
	else
	{
		CurRestTime = GameStartTime + 61 * ab - CurServerTime;
	}

	if (CurRestTime < 0)
	{
		ab++;
		min--;
	}

	if(HUD == nullptr)
		HUD = Cast<AMainHUD>(GetHUD());

	if (HUD)
	{
		HUD->DrawHUDTime(CurRestTime, min);
	}

	if (min < 5 && WaitTime)
	{
		EnableInputMode();
		WaitTime = false;
	}
		
}

void AMainPlayerController::EnableInputMode_Implementation()
{
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