// Fill out your copyright notice in the Description page of Project Settings.


#include "MechanicBrick.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Containers/UnrealString.h"
#include "Engine/StaticMeshSocket.h"
#include "../character/UserCharacter.h"

AMechanicBrick::AMechanicBrick() {



    Add = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Add"));
    Add->SetMobility(EComponentMobility::Movable);  // Set as movable

    Add->SetupAttachment(RootComponent);

    Constrait = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Constrait"));
    Constrait->SetupAttachment(RootComponent);

    AttachPoint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Attach"));
    AttachPoint->SetupAttachment(RootComponent);

    Add->SetSimulatePhysics(true);
    Brick->SetSimulatePhysics(true);
    Brick->SetMobility(EComponentMobility::Movable);  // Set as movable


    USceneComponent* scene;

    while (plugs.size() > 0) {
        scene = plugs.front();
        scene->DestroyComponent();
        plugs.pop();
    }

    for (int i = 0; i < 10; i++) {

        FString scene_name = "constrait";
        scene_name.AppendInt(i);

        scene = CreateDefaultSubobject<USceneComponent>(FName(scene_name));
        scene->SetupAttachment(Add);
        plugs.push(scene);

    }

    Healt_ = 1000;
    Height_Offset = 20;
    Machine_Running = false;
    Cable_Hook = false;
    ApplyForce_ToAdd = true;
}

void AMechanicBrick::setHeightOffset(int Offset)
{
    Height_Offset = Offset;
}


void AMechanicBrick::turnOnOffMachine(AUserCharacter* Character)
{
    if (Machine_Running) {
        Machine_Running = false;
    }
    else {
        Machine_Running = true;
    }

    interactWithMachine(Character);
}

void AMechanicBrick::setGhostComponent(UStaticMesh* Mesh)
{
    Ghost_Mesh = Mesh;
}

void AMechanicBrick::BeginPlay()
{
    Super::BeginPlay();

    Add_Local_Loc = Add->GetRelativeLocation();

    setPivot(Brick);
    Healt_ = 40;


}

void AMechanicBrick::setMaterialColor(FLinearColor Color)
{

    Brick->CreateDynamicMaterialInstance(0);
    Brick->SetVectorParameterValueOnMaterials(FName("BaseColor"), FVector(Color));
    Item_Color = FVector(Color);

    Add->CreateDynamicMaterialInstance(0);
    Add->SetVectorParameterValueOnMaterials(FName("BaseColor"), FVector(Color));
}
