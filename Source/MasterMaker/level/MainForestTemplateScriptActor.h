// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "../character/EnemyCharacter.h"
#include "../vehicle/EnemyVehicle.h"
#include <list>
#include <vector>
#include "MainForestTemplateScriptActor.generated.h"

/**
 *
 */
UCLASS()
class MASTERMAKER_API AMainForestTemplateScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

		std::list<AEnemyCharacter*> Spawned_Enemy_Characters;
	std::list<AEnemyVehicle*> Spawned_Enemy_Vehicles;
	FTimerHandle Spawn_Character_Timer;
	FTimerHandle Spawn_Vehicle_Timer;
	std::vector<FVector> Spawn_Points;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	int Level_Character_Spawn_Count;
	int Level_Vehicle_Spawn_Count;

	int Level_;
	bool Game_Playing;
	int Healt_;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void enemyDied(AActor* Enemy);
	void enemySpawned(AActor* Enemy);
	void startNewEnemyWaveIfEnded();
	void startEnemyWave(int Tier);
	void spawnEnemyCharacter(const FVector& SpawnLocation, float Radius);
	void spawnEnemyVehicle(const FVector& SpawnLocation, float Radius);
	void startGame();
	void stopGame();


};
