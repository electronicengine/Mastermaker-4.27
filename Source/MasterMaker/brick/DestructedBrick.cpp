// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructedBrick.h"
#include "Brick.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "../MasterMakerGameInstance.h"

// Sets default values
ADestructedBrick::ADestructedBrick()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    InitialLifeSpan = 2;
    Default_Root = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
    SetRootComponent(Default_Root);
    static ConstructorHelpers::FObjectFinder<UMaterial>
        material_asset(TEXT("Material'/Game/bricks/materials/brick_material.brick_material'"));
    Brick_Material = material_asset.Object;

    static ConstructorHelpers::FObjectFinder<UMaterial>
        skin_asset(TEXT("Material'/Game/characters/materials/skin.skin'"));
    CubeMan_Skin_Material = skin_asset.Object;

    static ConstructorHelpers::FObjectFinder<UMaterial>
        shirt_asset(TEXT("Material'/Game/characters/materials/shirt.shirt'"));
    CubeMan_Shirt_Material = shirt_asset.Object;

    static ConstructorHelpers::FObjectFinder<UMaterial>
        pant_asset(TEXT("Material'/Game/characters/materials/pant.pant'"));
    CubeMan_Pant_Material = pant_asset.Object;

    static ConstructorHelpers::FObjectFinder<UMaterial>
        hair_asset(TEXT("Material'/Game/characters/materials/hair.hair'"));
    CubeMan_Hair_Material = hair_asset.Object;


    for (int i = 0; i < 100; i++) {
        FName name = FName(FString("Piece_") + FString::FromInt(i));
        UStaticMeshComponent* one_piece = CreateDefaultSubobject<UStaticMeshComponent>(name);
        one_piece->SetSimulatePhysics(true);
        one_piece->SetCollisionProfileName(FName("BlockAll"));
        one_piece->SetupAttachment(Default_Root);

        Pieces.Push(one_piece);

    }
}

// Called when the game starts or when spawned
void ADestructedBrick::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void ADestructedBrick::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void ADestructedBrick::setColor(FVector ColorVec)
{
    for (int i = 0; i < Pieces.Num(); i++) {
        Pieces[i]->CreateDynamicMaterialInstance(0);
        Pieces[i]->SetVectorParameterValueOnMaterials(FName("BaseColor"), FVector(ColorVec));
    }

}


void ADestructedBrick::setImpactPointAndVelocity(const FVector& ImpactPoint, const FVector& Velocity)
{

    for (UStaticMeshComponent* comp : Pieces)
    {
        if (Name_.Find(CHARACTER_APPENDIX) >= 0) {
            comp->AddImpulseAtLocation(Velocity * 0.01, ImpactPoint);
            //comp->AddForceAtLocation(force, ImpactPoint);
            //comp->AddRadialImpulse(ImpactPoint, 100, Velocity.Size(), ERadialImpulseFalloff::RIF_Linear, false);
        }

    }
}

void ADestructedBrick::setBrickMesh(const TArray<UStaticMesh*>& Meshes)
{
    int i = 0;
    for (UStaticMesh* asset : Meshes)
    {
        UStaticMesh* mesh = Cast<UStaticMesh>(asset);
        Pieces[i]->SetSimulatePhysics(true);

        Pieces[i]->SetStaticMesh(mesh);
        Pieces[i++]->SetMaterial(0, Brick_Material);
    }
    for (int l = i; l < Pieces.Num(); l++) {
        Pieces[l]->DestroyComponent();
    }
}

void ADestructedBrick::setVehicleMesh(const TArray<UStaticMesh*>& Meshes)
{
    int i = 0;
    for (UStaticMesh* asset : Meshes)
    {
        UStaticMesh* mesh = Cast<UStaticMesh>(asset);
        Pieces[i]->SetSimulatePhysics(true);

        Pieces[i++]->SetStaticMesh(mesh);
    }
    for (int l = i; l < Pieces.Num(); l++) {
        Pieces[l]->DestroyComponent();
    }
}

void ADestructedBrick::setCubeManCharacterMesh(const TArray<UStaticMesh*>& Meshes)
{

    int i = 0;
    for (UStaticMesh* asset : Meshes)
    {
        UStaticMesh* mesh = Cast<UStaticMesh>(asset);

        Pieces[i]->SetStaticMesh(mesh);
        Pieces[i]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Pieces[i]->SetSimulatePhysics(true);

        TArray<FName> material_names = Pieces[i]->GetMaterialSlotNames();

        for (int k = 0; k < material_names.Num(); k++) {

            if (material_names[k].ToString().Find("skin") >= 0) {
                Pieces[i]->SetMaterial(k, CubeMan_Skin_Material);
            }
            if (material_names[k].ToString().Find("pant") >= 0) {
                Pieces[i]->SetMaterial(k, CubeMan_Pant_Material);
            }
            if (material_names[k].ToString().Find("shirt") >= 0) {
                Pieces[i]->SetMaterial(k, CubeMan_Shirt_Material);
            }
            if (material_names[k].ToString().Find("hair") >= 0) {
                Pieces[i]->SetMaterial(k, CubeMan_Hair_Material);
            }
            if (material_names[k].ToString().Find("Atlas") >= 0) {
                Pieces[i]->SetMaterial(k, CubeMan_Skin_Material);
            }

        }
        i++;
    }

    for(int l = i; l < Pieces.Num(); l++) {
        Pieces[l]->DestroyComponent();
    }
}



void ADestructedBrick::setMesh(FString Name)
{
    UMasterMakerGameInstance* instance = Cast<UMasterMakerGameInstance>(GetGameInstance());
    TArray<UStaticMesh*> assets = instance->Destructed_Meshes[Name];
    Name_ = Name;
    if (Name.Find(CHARACTER_APPENDIX) >= 0) {
        setCubeManCharacterMesh(assets);
    }
    else if (Name.Find(VEHICLE_APPENDIX) >= 0) {
        setVehicleMesh(assets);
    }
    else {
        setBrickMesh(assets);
    }


}


