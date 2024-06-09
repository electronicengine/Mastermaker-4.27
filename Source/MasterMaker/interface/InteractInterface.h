// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BuilderInterface.h"
#include "../brick/MechanicBrick.h"

#include "InteractInterface.generated.h"


class AUserCharacter;
class AWeaponBrick;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractInterface : public UBuilderInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class MASTERMAKER_API IInteractInterface : public IBuilderInterface
{
    GENERATED_BODY()

protected:

    bool Is_In_Car;

public:

    IInteractInterface();
    void equip();
    void exitCar();

    bool carHasPassenger();

    void enterCar(AUserCharacter* LegoChar);


    int buyBrick(const FVector& Location, const FString& Name, int Id = -1);

    void enteredToCar(IBuilderInterface* ObjectNearby);
    void exitedFromCar();
    void openInventoryWidget();
    void saveAndLoad();
    void interactNearby(AActor* OtherActor);

    AMechanicBrick* Cable_Start_Item;
    AUserCharacter* Passenger_;
    std::map<int, AWeaponBrick*> Weapons;
    bool Builder_Mode_Enable;


};
