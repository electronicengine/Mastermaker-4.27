// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <iostream>
#include <vector>
#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "WheeledVehicleMovementComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

//#include "CarSeat.h"
#include "../brick/weapon/WeaponBrick.h"
#include "../character/UserCharacter.h"
#include "../brick/Brick.h"
#include "../interface/ComposerInterface.h"
#include "../interface/PlugInterface.h"
#include "../interface/InteractInterface.h"
#include "../interface/BreakableInterface.h"
#include "VehicleBase.generated.h"


#define BRICK_LENGHT        50

/**
 *
 */
UCLASS()
class MASTERMAKER_API AVehicleBase : public AWheeledVehicle, public IComposerInterface, public IPlugInterface, public IInteractInterface, public IBreakableInterface
{
    GENERATED_BODY()


        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        USpringArmComponent* SpringArm;


    FVector Initial_Touch_Location;

    void setupWheels();
    void setupMaterials();
    float Last_Touch;
protected:
    virtual USceneComponent* CreatePluginPoint(FString Name) override;


    //ACarSeat* Car_Seat;

    int Current_Weapon_Index;

    bool Weapon_Camera_Used;
    int Current_Camera_Index;

public:
    AVehicleBase();

    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaSeconds) override;



    void moveForward(float Value);
    void moveRight(float Value);
    void turn(float Value);
    void lookUp(float Value);
    void interact();
    void OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location);
    void OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location);
    void TouchMoved(ETouchIndex::Type FingerIndex, FVector Location);
    void fire();

    void jump();

    int removeWeaponToInventory(int Index);
    int addWeaponToInventory(AWeaponBrick* Weapon);
    //void addSeatToCar(ACarSeat* Seat);

    void checkVehicleTurnedDown();

    int Last_TurnedDown_Time;
    UPROPERTY(EditAnywhere, Category = "Team")
        int Team_Id;

    int Current_Plugin_Index;

    UPROPERTY(EditAnywhere)
        UPhysicsConstraintComponent* Current_Closest_Constrait;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCameraComponent* Camera;
};
