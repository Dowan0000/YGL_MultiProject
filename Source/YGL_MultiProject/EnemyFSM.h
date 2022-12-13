// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"


UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Attack,
	Damage,
	Die,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YGL_MULTIPROJECT_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
		EEnemyState mState = EEnemyState::Idle;

	void IdleState();

	void MoveState();

	void AttackState();

	void DamageState();

	void DieState();

	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float idleDelayTime = 2;

	float currentTime = 0;

	UPROPERTY(VisibleAnywhere, Category = fsm)
	class AMainCharacter* target;

	UPROPERTY()
	class AEnemy* me;

	UPROPERTY(EditAnywhere, Category = FSM)
	float attackRange = 150.0f;

	UPROPERTY(EditAnywhere, Category = FSM)
	float attackDelayTime = 2.0f;

	void OnDamageProcess();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	int hp = 5;

	UPROPERTY(EditAnywhere, Category = FSM)
	float damageDelayTime = 2.0f;

	UPROPERTY(EditAnywhere, Category = FSM)
	float dieSpeed = 50.0f;

	UPROPERTY()
	class UEnemyAnim* anim;

	UPROPERTY()
	class AAIController* ai;

	FVector randomPos;

	bool GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest);
		
};
