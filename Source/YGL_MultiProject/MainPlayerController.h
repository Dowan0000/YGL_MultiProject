// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class YGL_MULTIPROJECT_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMainPlayerController();

	virtual void Tick(float DeltaTimes) override;

protected:
	virtual void BeginPlay() override;

	void SetHUDTime();

	UFUNCTION(Server, Reliable)
		void ServerTime(float TimeFromClient);

	UFUNCTION(Client, Reliable)
		void ClientTime(float TimeFromServer, float ServerTime);

	UFUNCTION(BlueprintNativeEvent)
	void EnableInputMode();

private:
	float GameStartTime;
	float CurServerTime;

	bool WaitTime;

	float min;
	int ab;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AMainHUD* HUD;

public:
	UFUNCTION(BlueprintNativeEvent)
	void IsDead(APawn* DeadPawn);

};
