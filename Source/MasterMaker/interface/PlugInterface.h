// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/Object.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include <vector>
#include <map>
#include <list>
#include "PlugInterface.generated.h"

#define BRICK_LENGHT        50

class ABrick;
class AVehicleBase;
class AWeaponBrick;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlugInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class MASTERMAKER_API IPlugInterface
{
    GENERATED_BODY()




        // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

    TArray<USceneComponent*> Plugin_Points;

    std::map<int, ABrick*> Plugged_Items_OnIt;
    bool plugged;
    int Current_Plugin_Index;
    float Height_Offset;
    int Own_Plugin_Index;
    int Item_Index;
    int Weapon_Index;
    int Total_Plug_Item;
    uint8 Plugin_Number_Width;
    uint8 Plugin_Number_Height;
    uint8 Plugin_Number;
    AVehicleBase* Owner_Car;
    IPlugInterface* Owner_Item;
    FRotator Offset_Rotation;
    FVector Offset_Location;
    AWeaponBrick* Attached_Weapon;
    int Id_;

    virtual USceneComponent* CreatePluginPoint(FString Name) = 0;
    void setupPluginPoints(const FVector& Begining_Pivot, int PluginNumberWidth, int PluginNumberHeight);
    void setupSidePlugPoints(const FVector& Begining_Pivot, int PluginNumberWidth, int PluginNumberHeight);
    void setAttachedWeapon(AWeaponBrick* Object);

    float calculateDistance(const FVector& Vector1, const FVector& Vector2);
    int getClosestPluginIndex(const std::vector<float>& Array);
    FVector getPlugin(const FVector& ImpactPoint);
    void plugTheItem(ABrick* Object, int PluginIndex, const FRotator& OffsetRotation, const FVector& OffsetLocation);
    void putTheItem(const FVector& Location, const FRotator& Rotation);


    bool highLightPlugin(UStaticMeshComponent* Ghost_Brick, UMaterial* Possible_Material, UMaterial* ImPossible_Material,
        ABrick* Interactable_Brick, const FHitResult& OutHit, const FRotator& OffetRotation, const FVector& OffetLocation, ABrick* OverlappedBrick);
    FRotator getPluginRotation();
    bool checkPluginPointAvailable(FVector& Point);
    void autoPlugin(AActor* TargetBrick);
    void detachItemsOnIt();
    void attachItem(ABrick* Object, const FVector& Location, const FRotator& Rotation, const FVector& OffsetLocation, const FRotator& OffsetRotation);
    void setOwner(IPlugInterface* Object);
    void addWeapon(ABrick* Object);
    void addSeat(ABrick* Object);
    void addPluggedItemList(ABrick* Object, const FVector& OffsetLocation, const FRotator& OffsetRotation, int PluginIndex);
    void notifyToOwnerItemDestroyed(int Id);
    void setId(int Id);
    int getId() { return Id_; }
};
