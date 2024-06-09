// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "brick/Brick.h"
#include "brick/weapon/WeaponBrick.h"
#include "vehicle/EnemyVehicle.h"
#include "brick/MechanicBrick.h"
#include <map>
#include "character/EnemyCharacter.h"
#include "MasterMakerGameInstance.generated.h"


#define WEAPON_APPENDIX             "weapon"
#define CLUSTER_WEAPON_APPENDIX     "cluster"
#define EXPLOSIVE_WEAPON_APPENDIX   "explosive"
#define FIRE_WEAPON_APPENDIX        "fire"
#define MELEE_WEAPON_APPENDIX       "melee"
#define VEHICLE_APPENDIX            "Vehicle"
#define CAR_APPENDIX                "CarChasis"
#define CHARACTER_APPENDIX          "Character"
#define MECHANIC_APPENDIX           "mechanic"
#define AI_APPENDIX                 "AI"
#define SIDED_APPENDIX              "sided"
#define LOW_APPENDIX                "Low"
#define HIGH_APPENDIX               "High"





static FORCEINLINE void LoadBlueprintFromPath(const FName& Path, std::map<FString, UBlueprintGeneratedClass*>& Array)
{
    TArray<UObject*> tempArray;
    if (EngineUtils::FindOrLoadAssetsByPath(*Path.ToString(), tempArray, EngineUtils::ATL_Class))
    {
        for (int i = 0; i < tempArray.Num(); ++i)
        {
            UObject* temp = tempArray[i];

            if (temp == NULL || (!Cast<UBlueprintGeneratedClass>(temp)))
            {
                continue;
            }
            Array[temp->GetName()] = Cast<UBlueprintGeneratedClass>(temp);

        }
    }
}


struct ItemOptions {

    FString Name;
    FLinearColor Color;
    UStaticMesh* Mesh;
    UMaterial* Material;
    int Health;

    ItemType Type;
    BrickSubType SubType;
};

/**
 *
 */
UCLASS()
class MASTERMAKER_API UMasterMakerGameInstance : public UGameInstance
{
    GENERATED_BODY()

        UPROPERTY(EditAnywhere, Category = "UI")
        TSubclassOf<class UUserWidget> Multiplayer_Panel_Container;

    UPROPERTY(EditAnywhere, Category = "UI")
        TSubclassOf<class UUserWidget> SandBox_Panel_Container;

    UPROPERTY(EditAnywhere, Category = "UI")
        TSubclassOf<class UUserWidget> SaveGame_Panel_Container;

    UPROPERTY(EditAnywhere, Category = "UI")
        TSubclassOf<class UUserWidget> GamePlay_Panel_Container;


    UBlueprintGeneratedClass* GeneratedClass;


    std::map<FString, UStaticMesh*> General_Brick_Meshes;
    std::map<FString, UStaticMesh*> Weapon_Meshes;
    std::map<FString, UStaticMesh*> Vehicle_Meshes;
    std::map<FString, UBlueprintGeneratedClass*> Mechanic_Classes;
    std::map<FString, UBlueprintGeneratedClass*> Vehicle_Classes;
    std::map<FString, UBlueprintGeneratedClass*> Character_Classes;

    std::map<FString, ItemOptions> Item_Options;

    UMaterial* Default_Brick_Material;


public:
    UMasterMakerGameInstance();

    void initializeItemOptions();
    void initializeDestructedMeshes();


    IComposerInterface* Saved_Item;
    FVector Impact_Point;
    std::map<FString, TArray<UStaticMesh*>> Destructed_Meshes;
    FString Selected_Item;
    FLinearColor Selected_Color;

    UUserWidget* Multiplayer_Panel;
    UUserWidget* SaveGame_Panel;
    UUserWidget* SandBox_Panel;
    UUserWidget* GamePlay_Panel;

    UUserWidget* savePanel(IComposerInterface* Item, FVector ImpactPoint);

    UUserWidget* loadSandBoxPanel();
    UUserWidget* loadCharacterPanel();
    UUserWidget* loadMultiplayerPanel();
    ABrick* spawnBrick(const FString& Name, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params = FActorSpawnParameters());
    AVehicleBase* spawnVehicle(const FString& Name, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params = FActorSpawnParameters());
    AEnemyCharacter* spawnCharacter(const FString& Name, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params = FActorSpawnParameters());

    AWeaponBrick* spawnWeapon(const FString& Name, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params = FActorSpawnParameters());
    AMechanicBrick* spawnMechanic(const FString& Name, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params = FActorSpawnParameters());

    AActor* spawnItem(const FVector& SpawnLocation, const FRotator& SpawnRotation, const FString& Name = "", const FVector& ImpactPoint = FVector(0, 0, 0), FActorSpawnParameters Params = FActorSpawnParameters());

    void selectCurrentProductItem(const FString& ItemName);
    void selectCurrentProductColor(FLinearColor Color);
    bool saveGame(FString Name);
    bool loadGame(FString Name);
    void setScreenMessage(const FString& Message, float TimeOut = 10.0f);
    void hideScreenMessage();
    AActor* findActorByName(const FString& ActorName);
    float findSuitableHeight(AActor* Actor);

    UTexture2D* texture;
    UBlueprintGeneratedClass* Spawned;

};
