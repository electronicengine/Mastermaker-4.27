// Fill out your copyright notice in the Description page of Project Settings.


#include "Brick.h"
#include <vector>
#include "Engine/Engine.h"
#include "../vehicle/VehicleBase.h"
#include "../MasterMakerGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Containers/UnrealString.h"
#include "Engine/StaticMeshSocket.h"
#include "../character/UserPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"



// Sets default values
ABrick::ABrick()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    Brick = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Brick"));
    SetRootComponent(Brick);
    Brick->SetSimulatePhysics(true);
    Brick->SetCollisionProfileName(FName("BlockAll"));
    //Brick->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
    Brick->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Yes;

    Breaked_ = false;
    plugged = false;

    for (int i = 0; i < 400; i++) {

        FString scene_name = "const";
        scene_name.AppendInt(i);

        USceneComponent* scene;
        scene = CreateDefaultSubobject<USceneComponent>(FName(scene_name));
        scene->SetupAttachment(Brick);
        plugs.push(scene);

    }


}


// Called when the game starts or when spawned
void ABrick::BeginPlay()
{
    Super::BeginPlay();
    First_Hit = true;

}


USceneComponent* ABrick::CreatePluginPoint(FString Name)
{
    USceneComponent* scene;
    //scene = CreateDefaultSubobject<USceneComponent>(FName(Name));
    //scene->SetupAttachment(Brick);

    /**/
    if (Name == "last")
    {
        while (plugs.size() > 0) {
            scene = plugs.front();
            scene->DestroyComponent();
            plugs.pop();
        }

        return nullptr;
    }

    scene = plugs.front();
    plugs.pop();

    return scene;
}


void ABrick::enablePhysics(bool Value)
{
    if (Brick)
        Brick->SetSimulatePhysics(Value);

}

void ABrick::setCollisionProfile(FString Profile)
{
    if (Brick){
        Brick->SetCollisionProfileName(FName(Profile));
    }

    //if (Cast<AMechanicBrick>(this)) {
    //    Cast<AMechanicBrick>(this)->Add->SetCollisionProfileName(FName(Profile));
    //}

}




UStaticMesh* ABrick::getBrickMesh()
{
    return Ghost_Mesh;
}



void ABrick::setMaterialColor(FLinearColor Color)
{
    Brick->CreateDynamicMaterialInstance(0);
    Brick->SetVectorParameterValueOnMaterials(FName("BaseColor"), FVector(Color));
    Item_Color = FVector(Color);
}

void ABrick::setBrickTypeOptions(ItemOptions& Options)
{

    Brick->SetStaticMesh(Options.Mesh);
    Ghost_Mesh = Options.Mesh;
    Brick_Mesh = Options.Mesh;
    Brick->SetMaterialByName(FName("main"), Options.Material);
    Material = Options.Material;

    Mass_ = Brick->GetMass();

    if (Options.Name.Find("Plate") >= 0) {
        Brick->SetMassOverrideInKg(NAME_None, 1000000, true);
        Brick->SetMassScale(NAME_None, 100000);
        Brick->bIgnoreRadialForce = true;
        Brick->bIgnoreRadialImpulse = true;

    }
    FVector Origin;
    FVector BoxExtent;
    float SphereRadius;

    UKismetSystemLibrary::GetComponentBounds(Brick, Origin, BoxExtent, SphereRadius);
    FVector center = Brick->GetCenterOfMass();

    setPivot(Brick);

    Current_Plugin_Index = 0;

    Brick->CreateDynamicMaterialInstance(0);
    Brick->SetVectorParameterValueOnMaterials(FName("BaseColor"), FVector(Options.Color));
    Item_Color = FVector(Options.Color);
    Item_Name = Options.Name;

}

void ABrick::setPivot(UStaticMeshComponent* Component)
{
    TArray<FName> names = Component->GetAllSocketNames();

    for (FName name : names) {

        const UStaticMeshSocket* soc = Component->GetSocketByName(name);
        FVector pivot = soc->RelativeLocation;

        FString tag = Component->GetSocketByName(name)->Tag;
        FString width = tag.Mid(0, tag.Find(":"));


        int plug_width = FCString::Atoi(*width.Mid(0, width.Find("x")));
        int plug_height = FCString::Atoi(*width.Right(width.Find("x")));
        FString plug_offset = tag.Mid(tag.Find(":") + 1, tag.Find(".") - tag.Find(":") - 1);
        FString healt = tag.Mid(tag.Find(".") + 1, tag.Len() - 1);

        if (name.ToString().Find("side") < 0) {
            setupPluginPoints(pivot, plug_width, plug_height);
            Height_Offset = FCString::Atoi(*plug_offset);
        }
        else {
            setupSidePlugPoints(pivot, plug_width, plug_height);
        }

        Healt_ = FCString::Atoi(*healt);

    }
}




void ABrick::setLegoCarOwner(AVehicleBase* Car)
{
    if (Car != nullptr)
        Owner_Car = Car;
}

void ABrick::applyForceToOwner(FVector Force, FVector Location)
{
    if (Owner_Item) {
        if (Cast<AMechanicBrick>(Owner_Item)) {
            if (Cast<AMechanicBrick>(Owner_Item)->ApplyForce_ToAdd) {
                Cast<AMechanicBrick>(Owner_Item)->Add->AddForceAtLocation(Force / 10, Location);
                Cast<AMechanicBrick>(Owner_Item)->applyForceToOwner(Force / 100, Location);
            }
            else {
                Cast<AMechanicBrick>(Owner_Item)->applyForceToOwner(Force, Location);

            }

        }
        else {
            Cast<ABrick>(Owner_Item)->applyForceToOwner(Force, Location);

        }
    }
    else {
        Brick->AddForceAtLocation(Force, Location);
    }
}




