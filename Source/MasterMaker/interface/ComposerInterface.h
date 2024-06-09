// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include <vector>
#include "ComposerInterface.generated.h"


class ABrick;
class UMasterMakerGameInstance;

USTRUCT()
struct FConstructionInfo
{
    GENERATED_BODY()

        int Plugin_Index;
    FRotator Offset_Rotation;
    FVector Offset_Location;
    FString Item_Name;
    FLinearColor Item_Color;
    class AActor* Item;
    std::vector<struct FConstructionInfo> Plugged_Items;
};



// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UComposerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MASTERMAKER_API IComposerInterface
{
	GENERATED_BODY()




        // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

    static FString Spawned_Vehicle_Name;
    static int Start_ID;

    FConstructionInfo compileConstructInfo(IComposerInterface* Object);
    static void buildFromConstructionInfo(const FConstructionInfo& Info, const FVector& BuildLocation,
        UMasterMakerGameInstance* GameInstance, ABrick* MainBrick = nullptr, bool IsServer = true);


};
