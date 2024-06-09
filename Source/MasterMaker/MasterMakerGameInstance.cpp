// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterMakerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/Button.h"
#include "vehicle/EnemyVehicleAIController.h"
#include "widget/SaveGameWidget.h"
#include "widget/GamePlayWidget.h"

#include "level/MasterMakerSaveGame.h"
#include "character/UserCharacter.h"
#include "character/UserPlayerState.h"


UMasterMakerGameInstance::UMasterMakerGameInstance()
{

    Selected_Item = "High2x2";
    SandBox_Panel = nullptr;
    GamePlay_Panel = nullptr;

    TArray<UObject*> general_brick_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/bricks/meshes"), general_brick_assets, EngineUtils::ATL_Regular);
    for (auto asset : general_brick_assets)
    {
        General_Brick_Meshes[asset->GetName()] = Cast<UStaticMesh>(asset);
    }

    TArray<UObject*> weapon_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/weapons/meshes"), weapon_assets, EngineUtils::ATL_Regular);
    for (auto asset : weapon_assets)
    {
        Weapon_Meshes[asset->GetName()] = Cast<UStaticMesh>(asset);
    }

    TArray<UObject*> vehicle_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/bricks/meshes/vehicle"), vehicle_assets, EngineUtils::ATL_Regular);
    for (auto asset : vehicle_assets)
    {
        General_Brick_Meshes[asset->GetName()] = Cast<UStaticMesh>(asset);
    }


    // default material
    static ConstructorHelpers::FObjectFinder<UMaterial>
        material_asset(TEXT("Material'/Game/bricks/materials/brick_material.brick_material'"));
    Default_Brick_Material = material_asset.Object;


    LoadBlueprintFromPath(FName("/Game/bricks/mechanic_bricks/"), Mechanic_Classes);
    LoadBlueprintFromPath(FName("/Game/characters/"), Character_Classes);
    LoadBlueprintFromPath(FName("/Game/vehicles/"), Vehicle_Classes);


    initializeItemOptions();
    initializeDestructedMeshes();


}

void UMasterMakerGameInstance::initializeItemOptions()
{

    for (auto mesh : General_Brick_Meshes)
    {
        Item_Options[mesh.first] = { mesh.first, Selected_Color, General_Brick_Meshes[mesh.first], Default_Brick_Material,10 };
    }

    for (auto mesh : Weapon_Meshes)
    {
        Item_Options[mesh.first] = { mesh.first, Selected_Color, Weapon_Meshes[mesh.first], Default_Brick_Material, 100 };
    }


}






void UMasterMakerGameInstance::initializeDestructedMeshes()
{

    for (std::pair<FString, UStaticMesh*> var : General_Brick_Meshes) {
        TArray<UObject*> MeshAssets;
        FString meshes_path = "/Game/bricks/destructed_meshes/";
        meshes_path += var.first;

        EngineUtils::FindOrLoadAssetsByPath(meshes_path, MeshAssets, EngineUtils::ATL_Regular);
        int i = 0;
        for (auto asset : MeshAssets)
        {
            UStaticMesh* mesh = Cast<UStaticMesh>(asset);
            Destructed_Meshes[var.first].Push(mesh);
        }
    }

}



UUserWidget* UMasterMakerGameInstance::savePanel(IComposerInterface* Item, FVector ImpactPoint)
{
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    UMasterMakerSaveGame* save_game = Cast<UMasterMakerSaveGame>(UGameplayStatics::CreateSaveGameObject(UMasterMakerSaveGame::StaticClass()));
    if (Item) {
        Saved_Item = Item;
    }
    Impact_Point = ImpactPoint;

    if (SaveGame_Panel == nullptr)
    {
        //if (GamePlay_Panel != nullptr)
        //{
        //    GamePlay_Panel->RemoveFromParent();
        //    GamePlay_Panel = nullptr;
        //}

        SaveGame_Panel = CreateWidget<UUserWidget>(this, SaveGame_Panel_Container);

        SaveGame_Panel->AddToViewport();


        controller->bShowMouseCursor = true;
        controller->bEnableClickEvents = true;
        controller->bEnableMouseOverEvents = true;
        controller->bEnableTouchEvents = true;

        SaveGame_Panel->SetVisibility(ESlateVisibility::Visible);

        controller->SetInputMode(FInputModeUIOnly());
    }
    else {

        if (SaveGame_Panel->GetVisibility() == ESlateVisibility::Visible)
        {
            SaveGame_Panel->SetVisibility(ESlateVisibility::Collapsed);
            controller->SetInputMode(FInputModeGameOnly());
            controller->bShowMouseCursor = false;
        }
        else {
            SaveGame_Panel->SetVisibility(ESlateVisibility::Visible);
            controller->SetInputMode(FInputModeUIOnly());

            controller->bShowMouseCursor = true;
        }

    }

    return SaveGame_Panel;
}

UUserWidget* UMasterMakerGameInstance::loadSandBoxPanel()
{
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);


    if (SandBox_Panel == nullptr)
    {
        /*     if(GamePlay_Panel != nullptr)
             {
                 GamePlay_Panel->RemoveFromParent();
                 GamePlay_Panel = nullptr;
             }*/

        SandBox_Panel = CreateWidget<UUserWidget>(this, SandBox_Panel_Container);

        SandBox_Panel->AddToViewport();


        controller->bShowMouseCursor = true;
        controller->bEnableClickEvents = true;
        controller->bEnableMouseOverEvents = true;
        controller->bEnableTouchEvents = true;

        SandBox_Panel->SetVisibility(ESlateVisibility::Visible);

        controller->SetInputMode(FInputModeUIOnly());
    }
    else {

        if (SandBox_Panel->GetVisibility() == ESlateVisibility::Visible)
        {
            SandBox_Panel->SetVisibility(ESlateVisibility::Collapsed);
            controller->SetInputMode(FInputModeGameOnly());
            controller->bShowMouseCursor = false;
        }
        else {
            SandBox_Panel->SetVisibility(ESlateVisibility::Visible);
            controller->SetInputMode(FInputModeUIOnly());

            controller->bShowMouseCursor = true;
        }

    }



    return SandBox_Panel;
}



UUserWidget* UMasterMakerGameInstance::loadCharacterPanel()
{

    if (GamePlay_Panel == nullptr)
    {
        if (SandBox_Panel != nullptr)
        {
            SandBox_Panel->RemoveFromParent();
            SandBox_Panel = nullptr;
        }

        GamePlay_Panel = CreateWidget<UUserWidget>(this, GamePlay_Panel_Container);

        if (GamePlay_Panel != nullptr)
            GamePlay_Panel->AddToViewport();

        APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

        controller->bShowMouseCursor = false;
        controller->bEnableClickEvents = true;
        controller->bEnableMouseOverEvents = true;
        controller->SetInputMode(FInputModeGameOnly());

#ifdef DESTOP_BUILD
        controller->ActivateTouchInterface(0);
#endif // DES


    }
    else {

        APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

        if (!GamePlay_Panel->IsVisible())
            GamePlay_Panel->SetVisibility(ESlateVisibility::Visible);

        controller->bShowMouseCursor = false;
        controller->bEnableClickEvents = true;
        controller->bEnableMouseOverEvents = true;
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
        controller->SetInputMode(InputMode);
        controller->bShowMouseCursor = false; // Optionally hide the mouse cursor

    }

    return GamePlay_Panel;
}

UUserWidget* UMasterMakerGameInstance::loadMultiplayerPanel()
{
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (Multiplayer_Panel == nullptr)
    {
        /*     if(GamePlay_Panel != nullptr)
             {
                 GamePlay_Panel->RemoveFromParent();
                 GamePlay_Panel = nullptr;
             }*/

        Multiplayer_Panel = CreateWidget<UUserWidget>(this, Multiplayer_Panel_Container);

        Multiplayer_Panel->AddToViewport();


        controller->bShowMouseCursor = true;
        controller->bEnableClickEvents = true;
        controller->bEnableMouseOverEvents = true;
        controller->bEnableTouchEvents = true;

        Multiplayer_Panel->SetVisibility(ESlateVisibility::Visible);

        controller->SetInputMode(FInputModeUIOnly());
    }
    else {
        if (Multiplayer_Panel->GetVisibility() == ESlateVisibility::Visible)
        {
            Multiplayer_Panel->SetVisibility(ESlateVisibility::Collapsed);
            controller->SetInputMode(FInputModeGameOnly());
            controller->bShowMouseCursor = false;
        }
        else {
            Multiplayer_Panel->SetVisibility(ESlateVisibility::Visible);
            controller->SetInputMode(FInputModeUIOnly());

            controller->bShowMouseCursor = true;
        }
    }

    return Multiplayer_Panel;
}



ABrick* UMasterMakerGameInstance::spawnBrick(const FString& Name, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params)
{

    ABrick* spawn_brick;

    spawn_brick = GetWorld()->SpawnActor<ABrick>(ABrick::StaticClass(), SpawnLocation, SpawnRotation, Params);
    Item_Options[Name].Color = Selected_Color;
    spawn_brick->setBrickTypeOptions(Item_Options[Name]);



    return spawn_brick;

}


AVehicleBase* UMasterMakerGameInstance::spawnVehicle(const FString& Name, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params)
{
    AVehicleBase* vehicle;
    FString name;
    int index;
    Name.FindChar('C', index);
    if (index >= 0) {
         name = Name.Left(index + 1);
    }
    else {
        name = FString("BP_") + Name + FString("_C");
    }

    vehicle = GetWorld()->SpawnActor<AVehicleBase>(Vehicle_Classes[name], SpawnLocation + FVector(0, 0, 100.0f), SpawnRotation, Params);

    return vehicle;
}

AEnemyCharacter* UMasterMakerGameInstance::spawnCharacter(const FString& Name, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params)
{
    AEnemyCharacter* chars;

    chars = GetWorld()->SpawnActor<AEnemyCharacter>(Character_Classes[FString("BP_") + Name + FString("_C")], SpawnLocation + FVector(0, 0, 100.0f), SpawnRotation, Params);

    return chars;
}

AWeaponBrick* UMasterMakerGameInstance::spawnWeapon(const FString& Name, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params)
{

    AWeaponBrick* weapon;
    weapon = GetWorld()->SpawnActor<AWeaponBrick>(AWeaponBrick::StaticClass(), SpawnLocation, SpawnRotation, Params);
    Item_Options[Name].Color = Selected_Color;
    weapon->setBrickTypeOptions(Item_Options[Name]);
    weapon->makePluginSettings();
    return weapon;
}

AMechanicBrick* UMasterMakerGameInstance::spawnMechanic(const FString& Name, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params)
{
    AMechanicBrick* mechanic;

    mechanic = GetWorld()->SpawnActor<AMechanicBrick>(Mechanic_Classes[FString("BP_") + Name + FString("_C")], SpawnLocation, SpawnRotation, Params);
    mechanic->setGhostComponent(General_Brick_Meshes[Name]);
    mechanic->Item_Name = Name;
    mechanic->setMaterialColor(Selected_Color);

    return mechanic;
}


float UMasterMakerGameInstance::findSuitableHeight(AActor* Actor)
{
    if (Actor) {
        FVector VehicleLocation = Actor->GetActorLocation();

        // Define the line trace start and end points
        FVector LineTraceStart = VehicleLocation;
        FVector LineTraceEnd = VehicleLocation + FVector(0.0f, 0.0f, -10000.0f); // Trace 10,000 units down
        FHitResult HitResult;
        float TerrainHeight = 0;
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            LineTraceStart,
            LineTraceEnd,
            ECC_Visibility // Trace against the visibility channel
        );

        if (bHit)
        {
            TerrainHeight = HitResult.Location.Z;
            return TerrainHeight + 40;

        }
        else {
            return 0;
        }


    }

    return 0;
}

AActor* UMasterMakerGameInstance::spawnItem(const FVector& SpawnLocation, const FRotator& SpawnRotation, const FString& Name, const FVector& ImpactPoint, FActorSpawnParameters Params)
{
    FString item_name;
    AWeaponBrick* weapon;
    AVehicleBase* vehicle;
    AMechanicBrick* machine;
    ABrick* brick;
    AEnemyCharacter* character;

    if (Name.GetAllocatedSize() != 0)
        item_name = Name;
    else
        item_name = Selected_Item;

    //weapon
    if (item_name.Find(WEAPON_APPENDIX) >= 0) {

        weapon = spawnWeapon(item_name, SpawnLocation, SpawnRotation, Params);
        return weapon;
    }
    else if (item_name.Find(VEHICLE_APPENDIX) >= 0) {
        vehicle = spawnVehicle(item_name, ImpactPoint, SpawnRotation, Params);
        return vehicle;
    }
    else if (item_name.Find(CHARACTER_APPENDIX) >= 0) {
        character = spawnCharacter(item_name, ImpactPoint, SpawnRotation, Params);
        return character;
    }
    else if (item_name.Find(MECHANIC_APPENDIX) >= 0) {
        machine = spawnMechanic(item_name, SpawnLocation + FVector(100, 0, 0), FRotator(0, 0, 0), Params);
        return machine;
    }
    else {
        brick = spawnBrick(item_name, SpawnLocation, SpawnRotation, Params);
        return brick;
    }

    return nullptr;
}



void UMasterMakerGameInstance::selectCurrentProductItem(const FString& ItemName)
{
    Selected_Item = ItemName;

}

void UMasterMakerGameInstance::selectCurrentProductColor(FLinearColor Color)
{
    Selected_Color = Color;
}

bool UMasterMakerGameInstance::saveGame(FString Name)
{
    UMasterMakerSaveGame* save_game = Cast<UMasterMakerSaveGame>(UGameplayStatics::CreateSaveGameObject(UMasterMakerSaveGame::StaticClass()));

    if (Saved_Item)
    {

        FConstructionInfo info = Saved_Item->compileConstructInfo(Saved_Item);

        TSharedPtr<FJsonObject> json = save_game->convertConstructionInfoToJson(info);

        save_game->SaveObject = save_game->serializeJsonObject(json);

        FString path = FPaths::ProjectContentDir() + FString("/json_data/") + Name + FString(".txt");
        FFileHelper::SaveStringToFile(save_game->SaveObject, *path);

        UGameplayStatics::SaveGameToSlot(save_game, Name, 0);
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("saved"));
        return true;

    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("save failed"));

        return false;
    }

}

bool UMasterMakerGameInstance::loadGame(FString Name)
{

    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    AUserCharacter* cha = Cast<AUserCharacter>(controller->GetPawn());
    if (cha) {
        Cast<AUserPlayerState>(cha->GetPlayerState())->load(Name, Impact_Point);
    }

    //    construction_info = save_game->Construction_Info;
   /* UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());

    UMasterMakerSaveGame* save_game = Cast<UMasterMakerSaveGame>(UGameplayStatics::CreateSaveGameObject(UMasterMakerSaveGame::StaticClass()));
    save_game = Cast<UMasterMakerSaveGame>(UGameplayStatics::LoadGameFromSlot(Name, 0));
    if (save_game) {
        TSharedPtr<FJsonObject> de_json = save_game->deserializeJsonObject(save_game->SaveObject);
        FConstructionInfo de_info = save_game->convertJsonToConstructionInfo(de_json);

        IComposerInterface::buildFromConstructionInfo(de_info, Impact_Point, game_instance);

        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("loaded"));
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, Name);
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "save game non");

    }*/


    return false;
}

void UMasterMakerGameInstance::setScreenMessage(const FString& Message, float TimeOut)
{
    if (Cast<UGamePlayWidget>(GamePlay_Panel)) {
        Cast<UGamePlayWidget>(GamePlay_Panel)->setMessage(Message, TimeOut);
    }

}

void UMasterMakerGameInstance::hideScreenMessage()
{

}


AActor* UMasterMakerGameInstance::findActorByName(const FString& ActorName)
{
    // Iterate through all actors in the level and find the one with the specified name
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

    for (int i = 0; i < FoundActors.Num(); i++)
    {
        AActor* Actor = FoundActors[i];
        if (Actor->GetName() == ActorName) {
            UE_LOG(LogTemp, Warning, TEXT("Found"));
            return Actor;
        }
    }

    // Actor with the specified name was not found
    return nullptr;
}