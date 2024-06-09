// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "../vehicle/EnemyVehicle.h"
#include "../interface/ComposerInterface.h"
#include "Json.h"

#include "MasterMakerSaveGame.generated.h"


/**
 *
 */
UCLASS()
class MASTERMAKER_API UMasterMakerSaveGame : public USaveGame
{
    GENERATED_BODY()

public:

    UMasterMakerSaveGame();

    //    UPROPERTY(EditAnywhere)
    FConstructionInfo Construction_Info;

    UPROPERTY(EditAnywhere)
        FString SaveObject;


    TSharedPtr<FJsonObject> convertConstructionInfoToJson(const FConstructionInfo& Info);
    FString serializeJsonObject(const TSharedPtr<FJsonObject>& Json);

    FConstructionInfo convertJsonToConstructionInfo(const TSharedPtr<FJsonObject>& Json);
    TSharedPtr<FJsonObject> deserializeJsonObject(const FString& JsonString);
};
