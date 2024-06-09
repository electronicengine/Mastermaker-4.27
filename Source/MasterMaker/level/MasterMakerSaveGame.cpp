// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterMakerSaveGame.h"



UMasterMakerSaveGame::UMasterMakerSaveGame()
{

}



TSharedPtr<FJsonObject> UMasterMakerSaveGame::convertConstructionInfoToJson(const FConstructionInfo& Info)
{

    TSharedPtr<FJsonObject> json = MakeShareable(new FJsonObject);
    TArray<TSharedPtr<FJsonValue>> plugged_bricks_onit;


    json->SetNumberField("Plugin_Index", Info.Plugin_Index);
    json->SetStringField("Offset_Rotation", Info.Offset_Rotation.ToString());
    json->SetStringField("Offset_Location", Info.Offset_Location.ToString());

    json->SetStringField("Item_Name", Info.Item_Name);
    json->SetStringField("Item_Color", Info.Item_Color.ToString());

    for (int i = 0; i < Info.Plugged_Items.size(); i++)
    {

        TSharedPtr<FJsonObject> brick_info = MakeShareable(new FJsonObject);
        brick_info = convertConstructionInfoToJson(Info.Plugged_Items[i]);

        plugged_bricks_onit.Push(MakeShareable(new FJsonValueObject(brick_info)));

    }

    json->SetArrayField("Plugged_Bricks", plugged_bricks_onit);


    return json;

}



FString UMasterMakerSaveGame::serializeJsonObject(const TSharedPtr<FJsonObject>& Json)
{
    FString json_string;

    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&json_string);

    FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);

    return json_string;
}



FConstructionInfo UMasterMakerSaveGame::convertJsonToConstructionInfo(const TSharedPtr<FJsonObject>& Json)
{
    FConstructionInfo main_brick_info;
    TArray<TSharedPtr<FJsonValue>> plugged_bricks_onit = Json->GetArrayField("Plugged_Bricks");

    main_brick_info.Plugin_Index = Json->GetIntegerField("Plugin_Index");
    main_brick_info.Offset_Rotation.InitFromString(Json->GetStringField("Offset_Rotation"));
    main_brick_info.Offset_Location.InitFromString(Json->GetStringField("Offset_Location"));
    main_brick_info.Item_Name = Json->GetStringField("Item_Name");
    main_brick_info.Item_Color.InitFromString(Json->GetStringField("Item_Color"));


    for (int i = 0; i < plugged_bricks_onit.Num(); i++)
    {
        FConstructionInfo brick_info;

        brick_info = convertJsonToConstructionInfo(plugged_bricks_onit[i]->AsObject());

        main_brick_info.Plugged_Items.push_back(brick_info);
    }

    return main_brick_info;
}



TSharedPtr<FJsonObject> UMasterMakerSaveGame::deserializeJsonObject(const FString& JsonString)
{


    TSharedPtr<FJsonObject> json = MakeShareable(new FJsonObject);

    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    FJsonSerializer::Deserialize(Reader, json);

    return json;
}

