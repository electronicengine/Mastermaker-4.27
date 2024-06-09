// Fill out your copyright notice in the Description page of Project Settings.


#include "PlugInterface.h"
#include "../brick/Brick.h"
#include "../brick/MechanicBrick.h"
#include "../brick/weapon/WeaponBrick.h"
#include "../vehicle/VehicleBase.h"
#include <vector>
#include "Engine/Engine.h"
#include "UObject/Object.h"
#include "Kismet/KismetSystemLibrary.h"


// Add default functionality here for any IPlugInterface functions that are not pure virtual.
void IPlugInterface::setupPluginPoints(const FVector& Begining_Pivot, int PluginNumberWidth, int PluginNumberHeight)
{

    FString scene_name("scene");
    USceneComponent* scene;
    FVector offset = FVector(0, 0, 0);

    int counter = 0;

    int plugin_number = PluginNumberWidth * PluginNumberHeight;

    for (int i = 0; i < plugin_number; i++)
    {

        scene_name = "Scene";
        scene_name.AppendInt(i);

        scene = CreatePluginPoint(scene_name);

        Plugin_Points.Push(scene);
    }

    // place Plugin Pivots
    for (int i = 0; i < PluginNumberHeight; i++)
    {
        for (int k = 0; k < PluginNumberWidth; k++)
        {
            Plugin_Points[counter++]->SetRelativeLocation(Begining_Pivot + offset);
            offset.Y += (BRICK_LENGHT / 2);
        }

        offset.X -= (BRICK_LENGHT / 2);
        offset.Y = 0;
    }

    Owner_Car = nullptr;
    Owner_Item = nullptr;
    Total_Plug_Item = 0;
}


void IPlugInterface::setupSidePlugPoints(const FVector& Begining_Pivot, int PluginNumberWidth, int PluginNumberHeight)
{

    FString scene_name("sidescene");
    USceneComponent* scene;
    FVector offset = FVector(0, 0, 0);

    int counter = Plugin_Points.Num();

    int plugin_number = PluginNumberWidth * PluginNumberHeight;

    for (int i = 0; i < plugin_number; i++)
    {

        scene_name = "sidescene";
        scene_name.AppendInt(i);

        scene = CreatePluginPoint(scene_name);
        scene->SetRelativeRotation(FRotator(0, 0, 90));

        Plugin_Points.Push(scene);
    }

    scene = CreatePluginPoint("last");


    // place Plugin Pivots
    for (int i = 0; i < PluginNumberHeight; i++)
    {
        for (int k = 0; k < PluginNumberWidth; k++)
        {
            Plugin_Points[counter++]->SetRelativeLocation(Begining_Pivot + offset);
            offset.Z += (BRICK_LENGHT / 2);
        }

        offset.X -= (BRICK_LENGHT / 2);
        offset.Z = 0;
    }
}


float IPlugInterface::calculateDistance(const FVector& Vector1, const FVector& Vector2)
{
    FVector delta = Vector1 - Vector2;

    return delta.Size();
}



bool IPlugInterface::highLightPlugin(UStaticMeshComponent* Ghost_Brick, UMaterial* Possible_Material, UMaterial* ImPossible_Material,
    ABrick* Interactable_Brick, const FHitResult& OutHit, const FRotator& OffetRotation, const FVector& OffetLocation, ABrick* OverlappedBrick)
{

    FVector plugin = getPlugin(OutHit.ImpactPoint);
    FRotator plugin_rotation = getPluginRotation();

    Ghost_Brick->SetStaticMesh(Interactable_Brick->getBrickMesh());
    //Ghost_Brick->SetWorldScale3D(FVector(0.85f, 0.85f, 0.85f));


    if (plugin != FVector(0, 0, 0))
    {


        Ghost_Brick->SetVisibility(true);

        Ghost_Brick->SetWorldLocationAndRotation(plugin,
            plugin_rotation);

        Ghost_Brick->AddLocalRotation(OffetRotation);

        Ghost_Brick->AddLocalOffset(FVector(0, 0, (Interactable_Brick->Height_Offset) * -1));
        Ghost_Brick->AddLocalOffset(OffetLocation);


        if (Ghost_Brick->IsOverlappingActor(OverlappedBrick))
        {

            Ghost_Brick->SetMaterial(0, ImPossible_Material);
            Ghost_Brick->SetMaterial(1, ImPossible_Material);

            return true;
        }
        else
        {
            Ghost_Brick->SetMaterial(0, Possible_Material);
            Ghost_Brick->SetMaterial(1, Possible_Material);

            return true;
        }
    }
    else
    {

        Ghost_Brick->SetVisibility(false);
        return false;
    }

}



FVector IPlugInterface::getPlugin(const FVector& ImpactPoint)
{

    if (Plugin_Points.Num() == 0)
        return FVector(0, 0, 0);

    float distance;
    std::vector<float> distance_array;


    for (int i = 0; i < Plugin_Points.Num(); i++)
    {
        distance = calculateDistance(ImpactPoint, (Plugin_Points[i]->GetComponentLocation()));

        distance_array.push_back(distance);
    }

    Current_Plugin_Index = getClosestPluginIndex(distance_array);

    return Plugin_Points[Current_Plugin_Index]->GetComponentLocation();

}



int IPlugInterface::getClosestPluginIndex(const std::vector<float>& Array)
{

    float dist = 4000000;
    int index = 0;

    for (int i = 0; i < Array.size(); i++)
    {
        if (Array[i] < dist)
        {
            index = i;
            dist = Array[i];
        }
    }

    return index;

}


void IPlugInterface::plugTheItem(ABrick* Object, int PluginIndex, const FRotator& OffsetRotation, const FVector& OffsetLocation)
{

    FVector plug_location;
    FRotator plug_rotation;

    if (PluginIndex < 0)
        PluginIndex = Current_Plugin_Index;


    if (PluginIndex < Plugin_Points.Num())
    {
        plug_location = Plugin_Points[PluginIndex]->GetComponentLocation();
        plug_rotation = Plugin_Points[PluginIndex]->GetComponentRotation();

        attachItem(Object, plug_location, plug_rotation, OffsetLocation, OffsetRotation);
        setOwner(Object);

        if (Cast<AWeaponBrick>(Object)) {
            addWeapon(Object);

        }

        addPluggedItemList(Object, OffsetLocation, OffsetRotation, PluginIndex);

    }
}

void IPlugInterface::putTheItem(const FVector& Location, const FRotator& Rotation)
{
    FVector mechanic_addcomp_location; 
    if (Cast<AMechanicBrick>(this)) {
        mechanic_addcomp_location = Cast<AMechanicBrick>(this)->Add->GetRelativeLocation();
    }

    Cast<ABrick>(this)->enablePhysics(false);
    Cast<ABrick>(this)->setCollisionProfile("OverlapAll");
    const FDetachmentTransformRules& attachment_rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
        EDetachmentRule::KeepWorld,
        EDetachmentRule::KeepWorld, false);
    Cast<ABrick>(this)->DetachFromActor(attachment_rules);
    Cast<ABrick>(this)->enablePhysics(false);
    Cast<ABrick>(this)->setCollisionProfile("OverlapAll");

    Cast<ABrick>(this)->SetActorLocationAndRotation(Location, Rotation,
        false, 0, ETeleportType::ResetPhysics);

    Cast<ABrick>(this)->enablePhysics(true);
    Cast<ABrick>(this)->setCollisionProfile("BlockAll");
    Cast<ABrick>(this)->SetActorEnableCollision(true);

}



void IPlugInterface::autoPlugin(AActor* BelowBrick)
{

    if (Cast<ABrick>(BelowBrick))
    {
        ABrick* below_brick = Cast<ABrick>(BelowBrick);
        FVector plugin = below_brick->getPlugin(Cast<ABrick>(this)->GetActorLocation());
        FRotator plugin_rotation = Cast<ABrick>(this)->GetActorRotation();

        //    SetActorLocationAndRotation(plugin, plugin_rotation);

        below_brick->plugTheItem(Cast<ABrick>(this), -1, plugin_rotation, FVector(0, 0, 0));
    }
    else if (Cast<AVehicleBase>(BelowBrick))
    {
        AVehicleBase* below_vehicle = Cast<AVehicleBase>(BelowBrick);
        FVector plugin = below_vehicle->getPlugin(Cast<ABrick>(this)->GetActorLocation());
        FRotator plugin_rotation = Cast<ABrick>(this)->GetActorRotation();


        //    SetActorLocationAndRotation(plugin, plugin_rotation);

        below_vehicle->plugTheItem(Cast<ABrick>(this), -1, plugin_rotation, FVector(0, 0, 0));
    }
}





FRotator IPlugInterface::getPluginRotation()
{
    if (Plugin_Points.Num() != 0)
        return Plugin_Points[Current_Plugin_Index]->GetComponentRotation();
    else
        return FRotator(0, 0, 0);
}

bool IPlugInterface::checkPluginPointAvailable(FVector& Point)
{



    for (int i = 0; i < Plugin_Points.Num(); i++) {

        int abs = (Point - Plugin_Points[i]->GetComponentLocation()).Size();
        if (abs == 0) {

            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue,
                FString::FromInt((Point - Plugin_Points[i]->GetComponentLocation()).Size()));


            return true;
        }
    }

    return false;
}



void IPlugInterface::detachItemsOnIt()
{
    AActor* item = Cast<AActor>(this);
    if (item) {

        if (Cast<ABrick>(item) || Cast<AVehicleBase>(item)) {
            for (auto child_item : Plugged_Items_OnIt) {
                if (child_item.second) {
                    if (Cast<AMechanicBrick>(child_item.second)) {
                        Cast<AMechanicBrick>(child_item.second)->Add->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,
                            EDetachmentRule::KeepWorld,
                            EDetachmentRule::KeepWorld, true));

                        Cast<AMechanicBrick>(child_item.second)->enablePhysics(true);
                        Cast<AMechanicBrick>(child_item.second)->setCollisionProfile("BlockAll");

                    }

                    if (Cast<AWeaponBrick>(child_item.second)) {
                        Cast<AWeaponBrick>(child_item.second)->removeOwner();
                    }

                    if (Cast<AVehicleBase>(item)) {
                        child_item.second->Owner_Car = nullptr;
                        child_item.second->Owner_Item = nullptr;

                    }
                    else {
                        child_item.second->Owner_Item = nullptr;
                    }
                }
            }
        }



        if (Cast<ABrick>(item)) {
            const FDetachmentTransformRules& detachment_rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
                EDetachmentRule::KeepWorld,
                EDetachmentRule::KeepWorld, true);
            Cast<AActor>(item)->DetachFromActor(detachment_rules);


            Cast<ABrick>(item)->enablePhysics(true);
            Cast<ABrick>(item)->setCollisionProfile("BlockAll");

            Cast<ABrick>(item)->Brick->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
        }


    }

}


void IPlugInterface::attachItem(ABrick* Object, const FVector& Location, const FRotator& Rotation, const FVector& OffsetLocation, const FRotator& OffsetRotation)
{

    const FDetachmentTransformRules& attachment_rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
        EDetachmentRule::KeepWorld,
        EDetachmentRule::KeepWorld, false);
    Object->DetachFromActor(attachment_rules);
    Object->enablePhysics(false);


    Object->SetActorLocationAndRotation(Location, Rotation,
        false, NULL, ETeleportType::ResetPhysics);

    Object->AddActorLocalRotation(OffsetRotation);
    Object->AddActorLocalOffset(OffsetLocation);

    Object->AddActorLocalOffset(FVector(0, 0, (Object->Height_Offset) * -1));

    if (Cast<AWeaponBrick>(Object)) {
        Cast<AWeaponBrick>(Object)->makePluginSettings();
    }
    else if (Cast<AMechanicBrick>(Object)) {
        if (Cast<ABrick>(this))
            Cast<AMechanicBrick>(Object)->AttachedComponent = Cast<ABrick>(this)->Brick;
    

    }
    else
    {
        Object->enablePhysics(true);
        Object->setCollisionProfile("BlockAll");
        Object->SetActorEnableCollision(true);
    }

    if (Cast<AMechanicBrick>(this)) {
        Object->AttachToComponent(Cast<AMechanicBrick>(this)->Add, FAttachmentTransformRules(EAttachmentRule::KeepWorld,
            EAttachmentRule::KeepWorld,
            EAttachmentRule::KeepWorld, true));


    }
    else {

        Object->AttachToComponent(Cast<AActor>(this)->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepWorld,
            EAttachmentRule::KeepWorld,
            EAttachmentRule::KeepWorld, true));


    }
}

void IPlugInterface::setAttachedWeapon(AWeaponBrick* Object) {

    Attached_Weapon = Object;

    if(Owner_Item)
        Owner_Item->setAttachedWeapon(Object);

}

void IPlugInterface::setOwner(IPlugInterface* Object)
{

    if (Owner_Car)
        Object->Owner_Car = Cast<AVehicleBase>(Owner_Car);
    else if (Cast<AVehicleBase>(this))
        Object->Owner_Car = Cast<AVehicleBase>(this);

    if (Cast<IPlugInterface>(this))
        Object->Owner_Item = this;
    else
        Object->Owner_Item = nullptr;

}

void IPlugInterface::addWeapon(ABrick* Object)
{
    int weapon_index = -1;
    if (Owner_Car) {
        weapon_index = Owner_Car->addWeaponToInventory(Cast<AWeaponBrick>(Object));
        Object->Weapon_Index = weapon_index;
        Cast<AWeaponBrick>(Object)->addedToVehicle(Owner_Car);
    }
    else if (Cast<AVehicleBase>(this)) {
        weapon_index = Cast<AVehicleBase>(this)->addWeaponToInventory(Cast<AWeaponBrick>(Object));
        Object->Weapon_Index = weapon_index;
        Cast<AWeaponBrick>(Object)->addedToVehicle(Cast<AVehicleBase>(this));
    }

    setAttachedWeapon(Cast<AWeaponBrick>(Object));
    

}

void IPlugInterface::addSeat(ABrick* Object)
{
    if (Object->GetName() == "CarSeat2x2")
    {
        //if (Owner_Car != nullptr)
        //    Owner_Car->addSeatToCar(Cast<ACarSeat>(Object));
        //else if (Cast<AVehicleBase>(this))
        //    Cast<AVehicleBase>(this)->addSeatToCar(Cast<ACarSeat>(Object));
    }
}

void IPlugInterface::addPluggedItemList(ABrick* Object, const FVector& OffsetLocation, const FRotator& OffsetRotation, int PluginIndex)
{

    Object->Own_Plugin_Index = PluginIndex;
    Object->Item_Index = Total_Plug_Item;
    Object->Offset_Rotation = OffsetRotation;
    Object->Offset_Location = OffsetLocation;
    Plugged_Items_OnIt.insert(std::make_pair(Total_Plug_Item, Object));
    Total_Plug_Item++;
}

void IPlugInterface::notifyToOwnerItemDestroyed(int Id)
{
    if (Owner_Item) {
        Owner_Item->Plugged_Items_OnIt.erase(Id);
    }

    if (Owner_Car && Attached_Weapon) {
        if(!Attached_Weapon->IsActorBeingDestroyed())
            Attached_Weapon->notifyVehicleDetached();
    }

}

void IPlugInterface::setId(int Id)
{
    Id_ = Id;
    if (Cast<IBreakableInterface>(this)) {
        Cast<IBreakableInterface>(this)->Destroyed_Id = Id;
    }
}




