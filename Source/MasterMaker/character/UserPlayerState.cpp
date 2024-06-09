// Fill out your copyright notice in the Description page of Project Settings.


#include "UserPlayerState.h"
#include "UserCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"                
#include "Engine/EngineTypes.h"         
#include "Components/PrimitiveComponent.h" 
#include "GameFramework/Pawn.h"           
#include "GameFramework/Actor.h"          
#include "UserPlayerState.h"              
#include "UserCharacter.h"                
#include "../brick/Brick.h"
#include "../MasterMakerGameInstance.h"
#include "../widget/GamePlayWidget.h"
#include "../level/MasterMakerSaveGame.h"
#include "../brick/weapon/WeaponBrick.h"

int AUserPlayerState::Current_Id_Counter = 1;

AUserPlayerState::AUserPlayerState()
{
}



void AUserPlayerState::fire()
{
    _Char = Cast<AUserCharacter>(GetPawn());

    if (_Char->Builder_Mode_Enable) {

        if (_Char->Target_Usable_Weapon) {
            FVector barrel_loc = _Char->Target_Usable_Weapon->getBarrelLocation();
            FVector barrel_dir = _Char->Target_Usable_Weapon->getBarrelDirection();
            vehicleFire(barrel_loc, barrel_dir);
        }
        else {
            FVector spawn_location = _Char->Builder_Camera->GetComponentLocation();
            FVector forward_vector = _Char->Builder_Camera->GetForwardVector().Rotation().Vector();

            fireOnServer(spawn_location, forward_vector);
        }


    }

}

void AUserPlayerState::buyBrick()
{
    _Char = Cast<AUserCharacter>(GetPawn());
    FLinearColor color = _Char->Game_Instance->Selected_Color;
    FVector spawn_location = _Char->Aim_Impact_Point;

    if (_Char->Sling_Mode_Enable) {
        buyBrickOnServer(_Char->Projectile_Location, color, _Char->Game_Instance->Selected_Item);
    }
    else {

        if (_Char->Game_Instance->Selected_Item.Find(MECHANIC_APPENDIX) >= 0) {
            spawn_location = _Char->GetMesh()->GetSocketLocation(TEXT("grab_socket"));
        }

        if (_Char->Grabbable_Brick == nullptr && _Char->Game_Instance) {

            buyBrickOnServer(spawn_location, color, _Char->Game_Instance->Selected_Item);
        }
    }


}

void AUserPlayerState::plugObject()
{
    _Char = Cast<AUserCharacter>(GetPawn());

    FRotator OffSet_Rotation(0, 0, 0);
    FVector OffSet_Location(0, 0, 0);
    int target_id = 0;
    if (_Char->Grabbable_Brick != nullptr && _Char->Building && _Char->Brick_Plugable)
    {
        if (_Char->Ghost->IsVisible())
        {
            int plugin_index = 0;
            if (_Char->Target_Plugable_Item) {
                target_id = Cast<IPlugInterface>(_Char->Target_Plugable_Item)->Id_;
                plugin_index = _Char->Target_Plugable_Item->Current_Plugin_Index;

            }

            FVector target_loc = _Char->Ghost->GetComponentLocation();
            FRotator target_rot = _Char->Ghost->GetComponentRotation();
            int grabbed_id = Cast<ABrick>(_Char->Grabbable_Brick)->Id_;

            plugObjectOnServer(_Char->Aim_Impact_Point, plugin_index, grabbed_id, target_id, target_rot, target_loc,
                _Char->OffSet_Rotation, _Char->OffSet_Location);

            _Char->Grabbable_Brick->Grabbed = false;
            _Char->Grabbable_Brick = nullptr;
            _Char->Object_NearBy = nullptr;
            _Char->Brick_Just_Plug = true;
            _Char->Keeping_Bricks = false;

            _Char->Ghost->SetVisibility(false);

        }

        _Char->Ghost->SetVisibility(false);


    }
}

void AUserPlayerState::interact()
{
    _Char = Cast<AUserCharacter>(GetPawn());

    if (Cast<AVehicleBase>(GetPawn())) {
        Cast<AVehicleBase>(GetPawn())->exitCar();
        if (GetNetMode() == ENetMode::NM_Client) {
            exitVehicleOnServer();
        }
    }
    else {
        if (_Char->Builder_Mode_Enable) {
            if (Cast<AWeaponBrick>(_Char->Target_Plugable_Item) && Cast<AWeaponBrick>(_Char->Target_Plugable_Item)->Type_ == WeaponType::fire) {
                selectWeapon();
            }
            else if (Cast<AMechanicBrick>(_Char->Target_Plugable_Item)) {
                selectConstraits();
            }
            else if (Cast<ABrick>(_Char->Target_Plugable_Item)) {
                selectBrick();
            }
        }
        else {
            if (Cast<AVehicleBase>(_Char->Object_NearBy)) {
                equipVehicle();
            }
            else if (Cast<ABrick>(_Char->Object_NearBy) && _Char->Object_NearBy != _Char->Grabbable_Brick) {
                equipBrick();
            }
            else {
                dropBrick();
            }
        }
    }

}

void AUserPlayerState::selectConstraits()
{
    if (!_Char->Cable_Start_Item && Cast<AMechanicBrick>(_Char->Target_Plugable_Item)->Cable_Hook) {
        _Char->Cable_Start_Item = Cast<AMechanicBrick>(_Char->Target_Plugable_Item);
        _Char->Game_Instance->setScreenMessage("Connect It A Item");
    }
    else if (_Char->Cable_Start_Item) {
        hookCableOnServer(_Char->Cable_Start_Item->Id_, _Char->Target_Plugable_Item->Id_);
        _Char->Cable_Start_Item = nullptr;

    }
    else {
        turnOnOffMachineOnServer(_Char->Target_Plugable_Item->Id_);
    }
}

void AUserPlayerState::selectWeapon()
{
    _Char->Target_Usable_Weapon = Cast<AWeaponBrick>(_Char->Target_Plugable_Item);
    _Char->Target_Usable_Weapon->useWeapon(_Char);
}

void AUserPlayerState::selectBrick()
{
    if (_Char->Cable_Start_Item) {
        if (Cast<ABrick>(_Char->Target_Plugable_Item)) {
            hookCableOnServer(_Char->Cable_Start_Item->Id_, _Char->Target_Plugable_Item->Id_);
            _Char->Cable_Start_Item = nullptr;
        }
    }
    else {
        if (_Char->Grabbable_Brick == nullptr)
        {
            if (Cast<ABrick>(_Char->Target_Plugable_Item)) {
                _Char->Grabbable_Brick = Cast<ABrick>(_Char->Target_Plugable_Item);
                equipBrickOnServer(_Char->Grabbable_Brick->Id_);
            }
        }
    }
}

void AUserPlayerState::equipVehicle()
{
    _Char = Cast<AUserCharacter>(GetPawn());

    if (_Char->Grabbable_Brick != nullptr)
        dropBrick();


    equipVehicleOnServer(Cast<AVehicleBase>(_Char->Object_NearBy)->Id_);

    Cast<UGamePlayWidget>(_Char->Game_Instance->GamePlay_Panel)->setInteractButtonVisibilty(ESlateVisibility::Visible);
    Cast<UGamePlayWidget>(_Char->Game_Instance->GamePlay_Panel)->setAimButtonVisibilty(ESlateVisibility::Hidden);

}

void AUserPlayerState::equipBrick()
{
    _Char = Cast<AUserCharacter>(GetPawn());

    if (_Char->Grabbable_Brick == Cast<ABrick>(_Char->Object_NearBy)) {
        dropBrick();
    }
    else if (_Char->Grabbable_Brick == nullptr)
    {
        _Char->Grabbable_Brick = Cast<ABrick>(_Char->Object_NearBy);
        equipBrickOnServer(Cast<ABrick>(_Char->Object_NearBy)->Id_);
    }
    else {

        dropBrick();
        _Char->Grabbable_Brick = Cast<ABrick>(_Char->Object_NearBy);
        equipBrickOnServer(Cast<ABrick>(_Char->Object_NearBy)->Id_);
    }

}

void AUserPlayerState::dropBrick()
{
    if (_Char->Grabbable_Brick != nullptr) {
        dropBrickOnServer(_Char->Grabbable_Brick->Id_);
    }
}

void AUserPlayerState::vehicleFire(const FVector& Location, const FVector& Direction)
{
    vehicleFireOnServer(Location, Direction);
}

void AUserPlayerState::throwItem(int Id, const FVector& ForceVector)
{
    throwItemOnServer(Id, ForceVector);
}

void AUserPlayerState::destroyedBrick(int Id)
{
    destroyedBrickOnServer(Id);
}

void AUserPlayerState::destroyedVehicle(int Id, const FVector& Location, const FRotator& Rotation)
{
    destroyedVehicleOnServer(Id, Location, Rotation);
}

void AUserPlayerState::load(const FString& Name, const FVector& Location)
{
    UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());

    UMasterMakerSaveGame* save_game = Cast<UMasterMakerSaveGame>(UGameplayStatics::CreateSaveGameObject(UMasterMakerSaveGame::StaticClass()));
    save_game = Cast<UMasterMakerSaveGame>(UGameplayStatics::LoadGameFromSlot(Name, 0));

    if (save_game) {

        UE_LOG(LogTemp, Warning, TEXT("%s"), *(save_game->SaveObject));
        loadOnServer(save_game->SaveObject, Location);
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("loaded"));
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, Name);
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "save game non");

    }
}


void AUserPlayerState::fireOnServer_Implementation(const FVector& Location, const FVector& ForwardVector)
{
    _Char = Cast<AUserCharacter>(GetPawn());

    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_DedicatedServer) {
        _Char->fire(Location, ForwardVector);
        fireOnMulticast(Location, ForwardVector);
    }
}

void AUserPlayerState::fireOnMulticast_Implementation(const FVector& Location, const FVector& ForwardVector)
{
    _Char = Cast<AUserCharacter>(GetPawn());

    if (GetNetMode() == ENetMode::NM_Client) {
        _Char->fire(Location, ForwardVector);
    }
}

void AUserPlayerState::buyBrickOnServer_Implementation(const FVector& Location, const FLinearColor& Color, const FString& Name)
{

    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_DedicatedServer) {
        int id = getUniq();


        if (Name.Find(VEHICLE_APPENDIX) < 0 && Name.Find(CHARACTER_APPENDIX) < 0) {
            _Char = Cast<AUserCharacter>(GetPawn());
            _Char->Game_Instance->Selected_Color = Color;
            _Char->buyBrick(Location, Name, id);
            buyBrickOnMulticast(Location, Color, Name, id);
        }
        else {
            FTimerHandle TimerHandle;
            UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());
            AActor* spawned_object = game_instance->spawnItem(Location, FRotator(0, 0, 0), Name, Location);

            if (Cast<IBreakableInterface>(spawned_object))
                Cast<IBreakableInterface>(spawned_object)->setId(id);

            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Location, Color, Name, id]()
                {
                    buyBrickOnMulticast(Location, Color, Name, id);
                }, 0.5, false);
        }

    }

}

void AUserPlayerState::buyBrickOnMulticast_Implementation(const FVector& Location, const FLinearColor& Color, const FString& Name, int Id)
{
    _Char = Cast<AUserCharacter>(GetPawn());

    if (GetNetMode() == ENetMode::NM_Client) {
        if (Name.Find(VEHICLE_APPENDIX) < 0 && Name.Find(CHARACTER_APPENDIX) < 0) {
            _Char->Game_Instance->Selected_Color = Color;
            _Char->buyBrick(Location, Name, Id);
        }
        else {
            IPlugInterface* breakable = findItemById<IPlugInterface>(0);
            if (breakable)
                breakable->setId(Id);

        }

    }
}

void AUserPlayerState::plugObjectOnServer_Implementation(const FVector& TargetPlugableLocation, int PluginIndex, int GrabbedId, int TargetId,
    const FRotator& TargetRotation, const FVector& TargetLocation, const FRotator& OffsetRotation, const FVector& OffsetLocation)
{
    _Char = Cast<AUserCharacter>(GetPawn());

    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_DedicatedServer) {
        IPlugInterface* pluggable = Cast<IPlugInterface>(_Char->Target_Plugable_Item);

        if (!pluggable) {
            pluggable = findItemById<IPlugInterface>(TargetId);
        }
        ABrick* grabbed = findItemById<ABrick>(GrabbedId);

        if (grabbed)
            _Char->plugObject(grabbed, pluggable, PluginIndex, TargetRotation, TargetLocation, OffsetRotation, OffsetLocation);

        plugObjectOnMulticast(TargetPlugableLocation, PluginIndex, GrabbedId, TargetId, TargetRotation, TargetLocation, OffsetRotation, OffsetLocation);

    }

}

void AUserPlayerState::plugObjectOnMulticast_Implementation(const FVector& TargetPlugableLocation, int PluginIndex, int GrabbedId, int TargetId,
    const FRotator& TargetRotation, const FVector& TargetLocation, const FRotator& OffsetRotation, const FVector& OffsetLocation)
{
    _Char = Cast<AUserCharacter>(GetPawn());

    if (GetNetMode() == ENetMode::NM_Client) {
        IPlugInterface* pluggable = Cast<IPlugInterface>(_Char->Target_Plugable_Item);
        if (!pluggable) {
            pluggable = findItemById<IPlugInterface>(TargetId);
        }

        ABrick* grabbed = findItemById<ABrick>(GrabbedId);

        if (grabbed)
            _Char->plugObject(grabbed, pluggable, PluginIndex, TargetRotation, TargetLocation, OffsetRotation, OffsetLocation);
    }
}

void AUserPlayerState::equipVehicleOnServer_Implementation(int Id)
{
    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_DedicatedServer) {
        AVehicleBase* object_nearby = findItemById<AVehicleBase>(Id);

        _Char->enteredToCar(object_nearby);

        equipVehicleOnMulticast(Id);
    }

}

void AUserPlayerState::equipVehicleOnMulticast_Implementation(int Id)
{
    if (GetNetMode() == ENetMode::NM_Client) {
        AVehicleBase* object_nearby = findItemById<AVehicleBase>(Id);
        _Char->enteredToCar(object_nearby);
    }
}

void AUserPlayerState::exitVehicleOnServer_Implementation()
{
    Cast<AVehicleBase>(GetPawn())->exitCar();
}


AActor* AUserPlayerState::findPlugableAtLocation(FVector Location, float SearchRadius)
{
    _Char = Cast<AUserCharacter>(GetPawn());

    FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SearchRadius);

    TArray<FOverlapResult> OverlapResults;
    GetWorld()->OverlapMultiByChannel(OverlapResults, Location, FQuat::Identity, ECC_Pawn, CollisionShape);

    for (const auto& result : OverlapResults)
    {
        AActor* plugable = result.GetActor();

        if (plugable)
        {
            return plugable;
        }
    }

    return nullptr;
}


void AUserPlayerState::vehicleFireOnServer_Implementation(const FVector& Location, const FVector& Direction)
{

    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_DedicatedServer) {
        AVehicleBase* vehicle = Cast<AVehicleBase>(GetPawn());
        if (vehicle) {
            if (!vehicle->Passenger_)
                vehicle->Camera->SetWorldRotation(FRotationMatrix::MakeFromX(Direction).Rotator());
        }

        ABullet* bullet_ptr = GetWorld()->SpawnActor<ABullet>(ABullet::StaticClass(), Location, FRotator(0, 0, 0));
        if (bullet_ptr != nullptr) {
            bullet_ptr->setMassAndStrenght(1, 1);
            bullet_ptr->addFireImpulse(Direction, 300);
        }

        vehicleFireOnMulticast(Location, Direction);
    }
}

void AUserPlayerState::vehicleFireOnMulticast_Implementation(const FVector& Location, const FVector& Direction)
{
    if (GetNetMode() == ENetMode::NM_Client) {
        AVehicleBase* vehicle = Cast<AVehicleBase>(GetPawn());
        if (vehicle) {
            if (!vehicle->Passenger_)
                vehicle->Camera->SetWorldRotation(FRotationMatrix::MakeFromX(Direction).Rotator());
        }

        ABullet* bullet_ptr = GetWorld()->SpawnActor<ABullet>(ABullet::StaticClass(), Location, FRotator(0, 0, 0));

        if (bullet_ptr != nullptr) {
            bullet_ptr->setMassAndStrenght(1, 1);
            bullet_ptr->addFireImpulse(Direction, 300);
        }
    }
}

void AUserPlayerState::destroyedBrickOnServer_Implementation(int DestroyedId)
{
    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_DedicatedServer) {

        IBreakableInterface* item = findItemById<IBreakableInterface>(DestroyedId);
        if (item)
            item->breakItem(FVector(0, 0, 0), FVector(0, 0, 0));

        destroyedBrickOnMulticast(DestroyedId);
    }
}

void AUserPlayerState::destroyedBrickOnMulticast_Implementation(int DestroyedId)
{
    if (GetNetMode() == ENetMode::NM_Client) {
        IBreakableInterface* item = findItemById<IBreakableInterface>(DestroyedId);
        if (item)
            item->breakItem(FVector(0, 0, 0), FVector(0, 0, 0));

    }
}

void AUserPlayerState::destroyedVehicleOnServer_Implementation(int Id, const FVector& Location, const FRotator& Rotation)
{

    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_DedicatedServer) {
        AVehicleBase* vehicle = findItemById<AVehicleBase>(Id);
        if (vehicle) {

            vehicle->exitCar();
            ADestructedBrick* dest_ptr = GetWorld()->SpawnActor<ADestructedBrick>(ADestructedBrick::StaticClass(), Location, Rotation);
            dest_ptr->setMesh("VehicleBase8x2");
            //dest_ptr->setColor(Brick_Color);

            //    const FDetachmentTransformRules &detachment_rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
            //                                                           EDetachmentRule::KeepWorld,
            //                                                           EDetachmentRule::KeepWorld, true);


            vehicle->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
            vehicle->detachItemsOnIt();
            vehicle->Destroy();
        }

        destroyedVehicleOnMulticast(Id, Location, Rotation);

    }

}

void AUserPlayerState::destroyedVehicleOnMulticast_Implementation(int Id, const FVector& Location, const FRotator& Rotation)
{
    if (GetNetMode() == ENetMode::NM_Client) {

        ADestructedBrick* dest_ptr = GetWorld()->SpawnActor<ADestructedBrick>(ADestructedBrick::StaticClass(), Location, Rotation);
        dest_ptr->setMesh("VehicleBase8x2");
    }

}

void AUserPlayerState::equipBrickOnServer_Implementation(int Id)
{
    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_DedicatedServer) {
        _Char = Cast<AUserCharacter>(GetPawn());

        ABrick* brick = findItemById<ABrick>(Id);

        if (brick)
            _Char->grabObject(brick);

        equipBrickOnMulticast(Id);
    }
}

void AUserPlayerState::equipBrickOnMulticast_Implementation(int Id)
{
    if (GetNetMode() == ENetMode::NM_Client) {
        _Char = Cast<AUserCharacter>(GetPawn());

        ABrick* brick = findItemById<ABrick>(Id);

        if (brick)
            _Char->grabObject(brick);

    }
}

void AUserPlayerState::dropBrickOnServer_Implementation(int Id)
{
    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_DedicatedServer) {
        _Char = Cast<AUserCharacter>(GetPawn());

        ABrick* brick = findItemById<ABrick>(Id);

        if (brick)
            _Char->dropObject(brick);

        dropBrickOnMulticast(Id);
    }
}
void AUserPlayerState::dropBrickOnMulticast_Implementation(int Id)
{
    if (GetNetMode() == ENetMode::NM_Client) {
        _Char = Cast<AUserCharacter>(GetPawn());

        ABrick* brick = findItemById<ABrick>(Id);

        if (brick)
            _Char->dropObject(brick);
    }
}

void AUserPlayerState::hookCableOnServer_Implementation(int ConstraitsId, int TargetId)
{
    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_DedicatedServer) {
        IPlugInterface* target = findItemById<IPlugInterface>(TargetId);
        AMechanicBrick* cable_start = findItemById<AMechanicBrick>(ConstraitsId);

        if (cable_start && target) {
            cable_start->hookCableToItem(Cast<ABrick>(target)->Brick);
        }
        hookCableOnMulticast(ConstraitsId, TargetId);
    }


}

void AUserPlayerState::hookCableOnMulticast_Implementation(int ConstraitsId, int TargetId)
{
    if (GetNetMode() == ENetMode::NM_Client) {
        IPlugInterface* target = findItemById<IPlugInterface>(TargetId);
        AMechanicBrick* cable_start = findItemById<AMechanicBrick>(ConstraitsId);

        if (cable_start && target) {
            cable_start->hookCableToItem(Cast<ABrick>(target)->Brick);
        }
    }
}

void AUserPlayerState::turnOnOffMachineOnServer_Implementation(int ConstraitsId)
{
    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_DedicatedServer) {
        AMechanicBrick* machine = findItemById<AMechanicBrick>(ConstraitsId);

        if (machine)
            Cast<AMechanicBrick>(machine)->turnOnOffMachine(Cast<AUserCharacter>(GetPawn()));

        turnOnOffMachineOnMulticast(ConstraitsId);

    }

}

void AUserPlayerState::turnOnOffMachineOnMulticast_Implementation(int ConstraitsId)
{
    if (GetNetMode() == ENetMode::NM_Client) {
        AMechanicBrick* machine = findItemById<AMechanicBrick>(ConstraitsId);

        if (machine)
            Cast<AMechanicBrick>(machine)->turnOnOffMachine(Cast<AUserCharacter>(GetPawn()));
    }
}

void AUserPlayerState::loadOnServer_Implementation(const FString& Info, const FVector& Location)
{
    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_DedicatedServer) {
        UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());
        UMasterMakerSaveGame* save_game = Cast<UMasterMakerSaveGame>(UGameplayStatics::CreateSaveGameObject(UMasterMakerSaveGame::StaticClass()));

        TSharedPtr<FJsonObject> de_json = save_game->deserializeJsonObject(Info);
        FConstructionInfo de_info = save_game->convertJsonToConstructionInfo(de_json);
        int id = getUniq();

        IComposerInterface::Start_ID = id;
        IComposerInterface::buildFromConstructionInfo(de_info, Location, game_instance);
        Current_Id_Counter += IComposerInterface::Start_ID;

        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Info, Location, id]()
            {
                loadOnMulticast(Info, Location, id);
            }, 1, false);

    }

}

void AUserPlayerState::loadOnMulticast_Implementation(const FString& Info, const FVector& Location, int StartId)
{
    if (GetNetMode() == ENetMode::NM_Client) {
        UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());
        UMasterMakerSaveGame* save_game = Cast<UMasterMakerSaveGame>(UGameplayStatics::CreateSaveGameObject(UMasterMakerSaveGame::StaticClass()));

        TSharedPtr<FJsonObject> de_json = save_game->deserializeJsonObject(Info);
        FConstructionInfo de_info = save_game->convertJsonToConstructionInfo(de_json);

        IComposerInterface::Start_ID = StartId;
        IComposerInterface::buildFromConstructionInfo(de_info, Location, game_instance, nullptr, false);
    }

}

void AUserPlayerState::throwItemOnServer_Implementation(int ItemId, const FVector& ForceVector)
{
    if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_DedicatedServer) {
        AActor* item = Cast<AActor>(findItemById<ABrick>(ItemId));
        _Char = Cast<AUserCharacter>(GetPawn());

        if (item)
            _Char->throwTheItem(ForceVector, item);

        throwItemOnMulticast(ItemId, ForceVector);
    }
}

void AUserPlayerState::throwItemOnMulticast_Implementation(int ItemId, const FVector& ForceVector)
{
    if (GetNetMode() == ENetMode::NM_Client) {
        AActor* item = Cast<AActor>(findItemById<ABrick>(ItemId));
        _Char = Cast<AUserCharacter>(GetPawn());

        if (item)
            _Char->throwTheItem(ForceVector, item);
    }
}
