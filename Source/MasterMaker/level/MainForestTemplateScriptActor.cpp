// Fill out your copyright notice in the Description page of Project Settings.


#include "MainForestTemplateScriptActor.h"
#include "../character/UserCharacter.h"
#include "../MasterMakerGameInstance.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "../character/UserPlayerState.h"

void AMainForestTemplateScriptActor::BeginPlay()
{
	Super::BeginPlay();

	Game_Playing = false;
	Level_ = 0;
	Level_Character_Spawn_Count = 0;
	Level_Vehicle_Spawn_Count = 0;

	Spawn_Points.push_back(FVector(-6646.248047, -22476.476562, -617.556763));
	Spawn_Points.push_back(FVector(-12590.478516, 1254.275635, 2749.819114));
	Spawn_Points.push_back(FVector(-5864.953613, -16635.958984, -521.38347));
}




void AMainForestTemplateScriptActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainForestTemplateScriptActor::enemyDied(AActor* Enemy)
{
	if (Cast<AEnemyCharacter>(Enemy)) {
		for (std::list<AEnemyCharacter*>::iterator it = Spawned_Enemy_Characters.begin(); it != Spawned_Enemy_Characters.end();)
		{
			if (*it == Cast<AEnemyCharacter>(Enemy))
				it = Spawned_Enemy_Characters.erase(it);
			else
				++it;
		}
	}
	else if (Cast<AEnemyVehicle>(Enemy)) {
		for (std::list<AEnemyVehicle*>::iterator it = Spawned_Enemy_Vehicles.begin(); it != Spawned_Enemy_Vehicles.end();)
		{
			if (*it == Cast<AEnemyVehicle>(Enemy))
				it = Spawned_Enemy_Vehicles.erase(it);
			else
				++it;
		}
	}

	startNewEnemyWaveIfEnded();
}

void AMainForestTemplateScriptActor::enemySpawned(AActor* Enemy)
{

	if (Cast<AEnemyCharacter>(Enemy)) {
		if (Level_Character_Spawn_Count == 0) {
			GetWorld()->GetTimerManager().ClearTimer(Spawn_Character_Timer);
		}
		else {
			Spawned_Enemy_Characters.push_back(Cast<AEnemyCharacter>(Enemy));
		}
	}
	else if (Cast<AEnemyVehicle>(Enemy)) {
		if (Level_Vehicle_Spawn_Count == 0) {
			GetWorld()->GetTimerManager().ClearTimer(Spawn_Vehicle_Timer);
		}
		else {
			Spawned_Enemy_Vehicles.push_back(Cast<AEnemyVehicle>(Enemy));
		}
	}
}

void AMainForestTemplateScriptActor::startNewEnemyWaveIfEnded()
{
	if (Spawned_Enemy_Vehicles.size() == 0 && Spawned_Enemy_Characters.size() == 0 && Game_Playing) {
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				startEnemyWave(++Level_);

			}, 3, false);
	}
}


void AMainForestTemplateScriptActor::startEnemyWave(int Tier)
{
	UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());
	AUserCharacter* charc = Cast<AUserCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	AVehicleBase* vehicle = Cast<AVehicleBase>(GetWorld()->GetFirstPlayerController()->GetPawn());

	game_instance->setScreenMessage(FString("Starting Level: ") + FString::FromInt(Level_));

	if (charc) {
		Level_Character_Spawn_Count = 42;
		if (Tier * 6 < Level_Character_Spawn_Count)
			Level_Character_Spawn_Count = Tier * 6;

		GetWorld()->GetTimerManager().SetTimer(Spawn_Character_Timer, [this]()
			{
				spawnEnemyCharacter(Spawn_Points[FMath::Rand() % 3], 400);

			}, 1, true);

	}
	else if (vehicle) {
		Level_Character_Spawn_Count = 42;
		if (Tier * 6 < Level_Character_Spawn_Count)
			Level_Character_Spawn_Count = Tier * 6;

		Level_Vehicle_Spawn_Count = 1;



		GetWorld()->GetTimerManager().SetTimer(Spawn_Character_Timer, [this]()
			{
				spawnEnemyCharacter(Spawn_Points[FMath::Rand() % 3], 400);

			}, 1, true);

		GetWorld()->GetTimerManager().SetTimer(Spawn_Vehicle_Timer, [this]()
			{
				spawnEnemyVehicle(Spawn_Points[FMath::Rand() % 3], 400);

			}, 4, true, Level_Character_Spawn_Count);

	}

}

void AMainForestTemplateScriptActor::spawnEnemyCharacter(const FVector& SpawnLocation, float Radius)
{

	AUserPlayerState* player_state = Cast<AUserPlayerState>(GetWorld()->GetFirstPlayerController()->GetPawn()->GetPlayerState());

	FVector RandomDirection = FMath::VRand();
	float RandomDistance = FMath::RandRange(0.0f, Radius);
	FVector RandomLocation = SpawnLocation + FVector(RandomDistance, RandomDistance, 0);

	player_state->buyBrickOnServer(RandomLocation, FLinearColor(0, 0, 0), "CubeManEnemyCharacter");
	Level_Character_Spawn_Count--;

}

void AMainForestTemplateScriptActor::spawnEnemyVehicle(const FVector& SpawnLocation, float Radius)
{

	AUserPlayerState* player_state = Cast<AUserPlayerState>(GetWorld()->GetFirstPlayerController()->GetPawn()->GetPlayerState());

	FVector RandomDirection = FMath::VRand();
	float RandomDistance = FMath::RandRange(0.0f, Radius);
	FVector RandomLocation = SpawnLocation + FVector(RandomDistance, RandomDistance, 0);


	FString path = FPaths::ProjectContentDir() + TEXT("json_data/saved_enemy_vehicle.txt");
	FString vehicle_data;

	if (FFileHelper::LoadFileToString(vehicle_data, *path))
	{
		player_state->loadOnServer(vehicle_data, RandomLocation + FVector(0, 0, 50));
	}

	Level_Vehicle_Spawn_Count--;

}

void AMainForestTemplateScriptActor::startGame()
{
	UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());

	Game_Playing = true;
	startEnemyWave(++Level_);
	game_instance->setScreenMessage(FString("Game Started"), 2);
}

void AMainForestTemplateScriptActor::stopGame()
{
	UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());

	Game_Playing = false;
	Level_ = 0;
	game_instance->setScreenMessage(FString("Game Stopped!"), 3);

}
