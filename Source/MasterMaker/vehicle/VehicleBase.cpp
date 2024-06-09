// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleBase.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "WheeledVehicleMovementComponent.h"
#include "VehicleAnimInstance.h"

#include "../widget/GamePlayWidget.h"
#include "../MasterMakerGameInstance.h"
#include "../interface/InteractInterface.h"
#include "../character/UserPlayerState.h"
#include "../level/MainForestTemplateScriptActor.h"

AVehicleBase::AVehicleBase()

{


    ////static ConstructorHelpers::FObjectFinder<USkeletalMesh>
    ////        mesh_asset(TEXT("SkeletalMesh'/Game/vehicles/skeleton/lego_car_chasis_1.lego_car_chasis_1'"));
    //static ConstructorHelpers::FObjectFinder<USkeletalMesh>
    //    mesh_asset(TEXT("SkeletalMesh'/Game/vehicles/skeleton/lego_car1.lego_car1'"));
    //USkeletalMesh* mesh = mesh_asset.Object;
    //GetMesh()->SetSkeletalMesh(mesh);

    //static ConstructorHelpers::FClassFinder<UAnimInstance>
    //    anim_bp(TEXT("AnimBlueprint'/Game/vehicles/anim/legocar1_anim.legocar1_anim_C'"));
    //GetMesh()->AnimClass = anim_bp.Class;
    //GetMesh()->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->SetRelativeLocation(FVector(-506.0f, 0, 170.f));

    setupMaterials();
    setupWheels();
    //setupPluginPoints(FVector(258.8, -122.7, 37.7f), 11, 23);
    setupPluginPoints(FVector(85.0f, -12.5f, 30.7f), 3, 9);
    bReplicates = true;
    Weapon_Camera_Used = false;
    Current_Plugin_Index = 0;
    //Car_Seat = nullptr;
    Current_Camera_Index = 0;
    Team_Id = 1;
    Weapon_Index = 0;
    Plugin_Number_Width = 3;
    Plugin_Number_Height = 9;
    Plugin_Number = Plugin_Number_Width * Plugin_Number_Height;
    Healt_ = 10;
    Last_TurnedDown_Time = 0;

    GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    Item_Name = "VehicleBase8x2";
    Last_Touch = 0;
}



void AVehicleBase::setupWheels()
{

    //TArray<FWheelSetup> CTyres;
    //FWheelSetup tire;
    //static ConstructorHelpers::FClassFinder<UVehicleWheel>
    //    front_tire(TEXT("Blueprint'/Game/vehicles/front_wheel.front_wheel_C'"));
    //static ConstructorHelpers::FClassFinder<UVehicleWheel>
    //    back_tire(TEXT("Blueprint'/Game/vehicles/back_wheel.back_wheel_C'"));

    //tire.WheelClass = front_tire.Class;
    //tire.BoneName = FName("front_left");
    ////tire.AdditionalOffset = FVector(173.104965f, -187.788605f, -57.286331f);
    //tire.bDisableSteering = false;
    //CTyres.Add(tire);

    //tire.WheelClass = front_tire.Class;
    //tire.BoneName = FName("front_right");
    ////tire.AdditionalOffset = FVector(172.294922f, 190.490829f, -57.286308f);
    //tire.bDisableSteering = false;
    //CTyres.Add(tire);

    //tire.WheelClass = back_tire.Class;
    //tire.BoneName = FName("back_left");
    ////tire.AdditionalOffset = FVector(-204.372986f, 190.490829f, -57.286308f);
    //tire.bDisableSteering = true;
    //CTyres.Add(tire);

    //tire.WheelClass = back_tire.Class;
    //tire.BoneName = FName("back_right");
    ////tire.AdditionalOffset = FVector(-204.372986f, -202.227158f, -57.286331f);
    //tire.bDisableSteering = true;
    //CTyres.Add(tire);

    //GetVehicleMovementComponent()->WheelSetups = CTyres;
    GetVehicleMovementComponent()->CreateVehicle();
    GetVehicleMovementComponent()->RecreatePhysicsState();
}



void AVehicleBase::setupMaterials()
{
    static ConstructorHelpers::FObjectFinder<UMaterial>
        material_asset0(TEXT("Material'/Game/vehicles/materials/chasis_material.chasis_material'"));
    UMaterial* chasis_material = material_asset0.Object;

    static ConstructorHelpers::FObjectFinder<UMaterial>
        material_asset1(TEXT("Material'/Game/vehicles/materials/wheel_material.wheel_material'"));
    UMaterial* wheel_material = material_asset1.Object;


    static ConstructorHelpers::FObjectFinder<UMaterial>
        material_asset2(TEXT("Material'/Game/vehicles/materials/root_material.root_material'"));
    UMaterial* root_material = material_asset2.Object;



    GetMesh()->SetMaterial(0, root_material);
    GetMesh()->SetMaterial(1, chasis_material);
    GetMesh()->SetMaterial(2, wheel_material);
    //GetMesh()->SetMaterial(3, wheel_material);
    //GetMesh()->SetMaterial(4, wheel_material);

}



USceneComponent* AVehicleBase::CreatePluginPoint(FString Name)
{
    USceneComponent* scene;
    scene = CreateDefaultSubobject<USceneComponent>(FName(Name));
    scene->SetupAttachment(GetMesh());
    return scene;
}



void AVehicleBase::BeginPlay()
{
    Super::BeginPlay();


    //    GetMesh()->SetSimulatePhysics(false);

    GetVehicleMovement()->SetHandbrakeInput(true);
    Game_Instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());
}




void AVehicleBase::jump()
{

    FVector as = GetMesh()->GetForwardVector();
    FVector asd = as.UpVector;

    GetMesh()->AddImpulse(asd * 1000000);
}



void AVehicleBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

    Super::SetupPlayerInputComponent(PlayerInputComponent);
    AUserPlayerState* player_state = Cast<AUserPlayerState>(GetPlayerState());

    PlayerInputComponent->BindAxis("MoveForward", this, &AVehicleBase::moveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AVehicleBase::moveRight);

    PlayerInputComponent->BindAxis("Turn", this, &AVehicleBase::turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AVehicleBase::lookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AVehicleBase::jump);

    PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AVehicleBase::interact);
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AVehicleBase::interact);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AVehicleBase::fire);

    PlayerInputComponent->BindTouch(IE_Pressed, this, &AVehicleBase::OnTouchPressed);
    PlayerInputComponent->BindTouch(IE_Released, this, &AVehicleBase::OnTouchReleased);
    PlayerInputComponent->BindTouch(IE_Repeat, this, &AVehicleBase::TouchMoved);

}


void AVehicleBase::OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location)
{

#ifndef DESKTOP_BUILD

    double CurrentTime = FPlatformTime::Seconds() * 1000.0;
    float TimeSinceLastTouch = CurrentTime - Last_Touch;

    if (TimeSinceLastTouch < 200 && Last_Touch != 0)
    {
        fire();
    }

    Last_Touch = CurrentTime;
#endif

    Initial_Touch_Location = Location;

}


void AVehicleBase::OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location)
{

     Initial_Touch_Location = FVector::ZeroVector;


}

void AVehicleBase::TouchMoved(ETouchIndex::Type FingerIndex, FVector Location)
{


#ifndef DESKTOP_BUILD

    FVector Delta = Location - Initial_Touch_Location;
    float Sensitivity = 0.1f;

    float YawChange = Delta.X * Sensitivity;
    float PitchChange = Delta.Y * Sensitivity;

    if (YawChange > 15 || PitchChange > 15)
        return;

    AddControllerYawInput(YawChange);
    AddControllerPitchInput(PitchChange);
    Initial_Touch_Location = Location;

#endif // DESKTOP_BUILD


}


void AVehicleBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    //checkVehicleTurnedDown();
}



void AVehicleBase::moveForward(float Value)
{
    GetVehicleMovement()->SetThrottleInput(Value);
}



void AVehicleBase::moveRight(float Value)
{
    GetVehicleMovement()->SetSteeringInput(Value);
}



void AVehicleBase::turn(float Value)
{
    AddControllerYawInput(Value);
}



void AVehicleBase::lookUp(float Value)
{
    AddControllerPitchInput(Value);
}



void AVehicleBase::interact()
{

    AUserPlayerState* player_state = Cast<AUserPlayerState>(GetPlayerState());
    player_state->interact();

}



void AVehicleBase::fire()
{

    static int weapon_index;


    for (std::pair<int, AWeaponBrick*> weapon : Weapons) {
        weapon.second->fire();

    }


}





int AVehicleBase::removeWeaponToInventory(int Index)
{
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString("removeWeaponToInventory : ") + FString::FromInt(Index));

    if (Weapons.size() > 0) {
        Weapons.erase(Index);
    }

    return 0;
}

int AVehicleBase::addWeaponToInventory(AWeaponBrick* Weapon)
{
    Current_Weapon_Index++;

    Weapons.emplace(std::make_pair(Current_Weapon_Index, Weapon));

    return Current_Weapon_Index;

}



//void AVehicleBase::addSeatToCar(ACarSeat* Seat)
//{
//    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("addSeatToCar"));
//
//    if (Seat != nullptr)
//        Car_Seat = Seat;
//}



void AVehicleBase::checkVehicleTurnedDown()
{

    FVector UpVector = GetActorUpVector();
    FVector WorldDownVector = -FVector::UpVector;
    float DotProduct = FVector::DotProduct(UpVector, WorldDownVector);
    FVector AngularVelocity = GetMesh()->GetPhysicsAngularVelocityInDegrees();
    float roll_rate = AngularVelocity.X;
    float pich_rate = AngularVelocity.Y;
    float yaw_rate = AngularVelocity.Z;
    float ThresholdSpeed = 400.0f;


    if (FMath::IsNearlyEqual(DotProduct, 1.0f, 0.5f))  // Vehicle is turned down (flipped)
    {
        Last_TurnedDown_Time = time(NULL);
    }

    if (time(NULL) - Last_TurnedDown_Time > 10 && Last_TurnedDown_Time != 0) {

        FVector current_location = GetActorLocation();
        float z = Game_Instance->findSuitableHeight(this);

        SetActorLocation(FVector(current_location.X, current_location.Y, z), false, nullptr, ETeleportType::TeleportPhysics);
        SetActorRotation(FRotator(0, 0, 0), ETeleportType::TeleportPhysics);

        GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector, false, NAME_None);
        GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector, false, NAME_None);

        Last_TurnedDown_Time = time(NULL);
    }

    if (FMath::Abs(yaw_rate) > ThresholdSpeed || FMath::Abs(pich_rate) > ThresholdSpeed || FMath::Abs(roll_rate) > ThresholdSpeed) // vehicle is keeping turn
    {

        float z = Game_Instance->findSuitableHeight(this);
        FVector current_location = GetActorLocation();
        SetActorLocation(FVector(current_location.X, current_location.Y, z), false, nullptr, ETeleportType::TeleportPhysics);
        SetActorRotation(FRotator(0, 0, 0), ETeleportType::TeleportPhysics);

        GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector, false, NAME_None);
        GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector, false, NAME_None);


    }


}