// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractInterface.h"
#include "../vehicle/VehicleBase.h"
#include "../MasterMakerGameInstance.h"
#include "../widget/GamePlayWidget.h"
#include "../character/UserCharacter.h"
#include "../vehicle/VehicleBase.h"
#include "Engine/StaticMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../brick/weapon/WeaponBrick.h"
#include "../widget/GamePlayWidget.h"
// Add default functionality here for any IInteractInterface functions that are not pure virtual.


IInteractInterface::IInteractInterface()
{
    Builder_Mode_Enable = false;

}


void IInteractInterface::equip()
{
    /*if (Builder_Mode_Enable) {
        if (Cast<AMechanicBrick>(Target_Plugable_Item)) {
            if (!Cable_Start_Item && Cast<AMechanicBrick>(Target_Plugable_Item)->Cable_Hook) {
                Cable_Start_Item = Cast<AMechanicBrick>(Target_Plugable_Item);
                Game_Instance->setScreenMessage("Connect It A Item");
            }
            else if(Cable_Start_Item) {
                Cable_Start_Item->hookCableToItem(Cast<ABrick>(Target_Plugable_Item)->Brick);
                Cable_Start_Item = nullptr;

            }else {
                Cast<AMechanicBrick>(Target_Plugable_Item)->turnOnOffMachine();
            }
        }
        else {
            if (Cable_Start_Item) {
                if (Cast<ABrick>(Target_Plugable_Item)) {
                    Cable_Start_Item->hookCableToItem(Cast<ABrick>(Target_Plugable_Item)->Brick);
                    Cable_Start_Item = nullptr;
                }

            }
            else {
                if (Grabbable_Brick == nullptr)
                {
                    if (Cast<ABrick>(Target_Plugable_Item)) {
                        Grabbable_Brick = Cast<ABrick>(Target_Plugable_Item);
                        grabObject(Grabbable_Brick);
                    }
                }
            }
        }

    }
    else {
        if (Cast<AVehicleBase>(this)) {
            exitCar();
        }
        else {

            if (Cast<AVehicleBase>(Object_NearBy))
            {
                enteredToCar();
                Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setInteractButtonVisibilty(ESlateVisibility::Visible);
                Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setAimButtonVisibilty(ESlateVisibility::Hidden);



            }
            else if (Cast<ABrick>(Object_NearBy))
            {
                if (Grabbable_Brick == Cast<ABrick>(Object_NearBy))
                    dropObject(Grabbable_Brick);
                else
                {
                    if (Grabbable_Brick == nullptr)
                    {
                        Grabbable_Brick = Cast<ABrick>(Object_NearBy);
                        grabObject(Grabbable_Brick);

                    }
                    else
                    {
                        dropObject(Grabbable_Brick);
                        Grabbable_Brick = Cast<ABrick>(Object_NearBy);

                        grabObject(Grabbable_Brick);

                    }
                }


            }
            else
            {
                if (Grabbable_Brick != nullptr)
                    dropObject(Grabbable_Brick);

            }

        }
    }*/


}




void IInteractInterface::enteredToCar(IBuilderInterface* ObjectNearby)
{

    if (ObjectNearby != nullptr)
    {
        AVehicleBase* interactable_vehicle = Cast<AVehicleBase>(ObjectNearby);

        if (interactable_vehicle != nullptr)
        {
            Cast<AUserCharacter>(this)->AddActorLocalOffset(FVector(0, 0, 100));
            Cast<AUserCharacter>(this)->SetActorHiddenInGame(true);

            interactable_vehicle->enterCar(Cast<AUserCharacter>(this));
            Is_In_Car = true;

        }
    }



}


int IInteractInterface::buyBrick(const FVector& Location, const FString& Name, int Id)
{
    FActorSpawnParameters params;
    params.Owner = Cast<ACharacter>(this);

    if (Cast<ISlingInterface>(this)->Sling_Mode_Enable && Name.Find("Sling") >= 0)
        return 0;

    AActor* spawned_object = Game_Instance->spawnItem(Location, FRotator(0, 0, 0), Name, Location, params);

    Grabbable_Brick = Cast<ABrick>(spawned_object);
    if (Cast<ISlingInterface>(this)->Sling_Mode_Enable && Grabbable_Brick != nullptr) {
        Grabbable_Brick->enablePhysics(false);
    }
    else if (Grabbable_Brick != nullptr)
    {
        grabObject(Grabbable_Brick);
        Brick_Just_Plug = false;
    }


   if (Cast<IPlugInterface>(spawned_object)) {
       Cast<IPlugInterface>(spawned_object)->setId(Id);
   }
   else if (Cast<IBreakableInterface>(spawned_object)) {
       Cast<IBreakableInterface>(spawned_object)->setId(Id);
    }

    return Id;

}


void IInteractInterface::exitedFromCar()
{
    Is_In_Car = false;
    Object_NearBy = nullptr;

    Cast<AUserCharacter>(this)->AddActorLocalOffset(FVector(0, -250, 0));
    Cast<AUserCharacter>(this)->GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
    Cast<AUserCharacter>(this)->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);

    Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->Button_Punch->SetVisibility(ESlateVisibility::Visible);

    Is_In_Car = false;
}


void IInteractInterface::openInventoryWidget()
{
    if (Game_Instance)
        Game_Instance->loadSandBoxPanel();

}

void IInteractInterface::saveAndLoad()
{

    if (Game_Instance) {
        Game_Instance->savePanel(Cast<IComposerInterface>(Target_Plugable_Item), Aim_Impact_Point);
    }
    else {

    }

}

void IInteractInterface::interactNearby(AActor* OtherActor)
{
    AVehicleBase* car = Cast<AVehicleBase>(OtherActor);
    if (car != nullptr) {
        Object_NearBy = car;
    }
    else
    {
        ABrick* brick = Cast<ABrick>(OtherActor);
        if (brick != nullptr) {

            if (brick->Owner_Car != nullptr) {
                Object_NearBy = brick->Owner_Car;
            }
            else {
                Object_NearBy = brick;
            }

        }
    }
}


bool IInteractInterface::carHasPassenger()
{
    if (Passenger_ == nullptr)
        return false;
    else
        return true;
}



void IInteractInterface::enterCar(AUserCharacter* LegoChar)
{
    //if(LegoChar != nullptr && Car_Seat != nullptr)
    //{

    Passenger_ = LegoChar;

    AController* lego_man_controller = Passenger_->GetController();

    ENetMode netMode = LegoChar->GetWorld()->GetNetMode();

    if (netMode == ENetMode::NM_ListenServer || netMode == ENetMode::NM_Standalone || netMode == ENetMode::NM_DedicatedServer)
        lego_man_controller->Possess(Cast<AVehicleBase>(this));

    if (Weapons.size() > 0) {

        Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setFireButtonVisibilty(ESlateVisibility::Visible);
        Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->Button_Punch->SetVisibility(ESlateVisibility::Hidden);


    }


}



void IInteractInterface::exitCar()
{

    if (Passenger_) {
        AController* car_controller = Cast<AVehicleBase>(this)->GetController();

        car_controller->Possess(Passenger_);

        //const FDetachmentTransformRules& attachment_rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
        //    EDetachmentRule::KeepWorld,
        //    EDetachmentRule::KeepWorld, true);
        /*Passenger_->DetachFromActor(attachment_rules);*/
        Passenger_->SetActorLocation(Cast<AVehicleBase>(this)->GetActorLocation());
        Passenger_->AddActorWorldOffset(FVector(100, 0, 10));
        Passenger_->SetActorHiddenInGame(false);

        Cast<IInteractInterface>(Passenger_)->exitedFromCar();

        Passenger_ = nullptr;

        Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setAimButtonVisibilty(ESlateVisibility::Visible);
        Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setFireButtonVisibilty(ESlateVisibility::Hidden);

        //    GetMesh()->SetSimulatePhysics(false);
    }


}
