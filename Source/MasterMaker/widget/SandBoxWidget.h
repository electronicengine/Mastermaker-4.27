// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include <vector>
#include <memory>
#include <map>

#include "SandBoxWidget.generated.h"


class UMasterMakerGameInstance;
static std::map<FString, FLinearColor> Colors = {

    {"Black", FLinearColor(0.005f, 0.005f, 0.005f)},
    {"Gray", FLinearColor(0.1f, 0.1f, 0.1f)},
    {"White", FLinearColor(0.7, 0.7, 0.7)},
    {"Yellow", FLinearColor(0.8125, 0.740577, 0.0)},
    {"Orange", FLinearColor(1.0, 0.309655, 0.0)},
    {"Brown", FLinearColor(0.3125, 0.062512, 0.0)},
    {"Red",FLinearColor(0.765625, 0.0, 0.005185)},
    {"Purple", FLinearColor(0.5, 0.0, 0.30656)},
    {"Blue", FLinearColor(0.0, 0.002649, 0.619792)},
    {"Cyan", FLinearColor(0.0, 0.445146, 0.604167)},
    {"Green",FLinearColor(0.039721, 0.401042, 0.0)},

};

enum BrickMenuCategory {
    General,
    Weapon,
    Vehicle,
    Building,
    Mechanic,
    character
};
/**
 *
 */
UCLASS()
class MASTERMAKER_API USandBoxWidget : public UUserWidget
{
    GENERATED_BODY()

        virtual bool Initialize();

    UMasterMakerGameInstance* Game_Instance;

    std::vector<std::pair<FString, UButton*>> GeneralBrick_Buttons;
    std::vector<std::pair<FString, UButton*>> WeaponBrick_Buttons;
    std::vector<std::pair<FString, UButton*>> VehicleBrick_Buttons;
    std::vector<std::pair<FString, UButton*>> BuildingBrick_Buttons;
    std::vector<std::pair<FString, UButton*>> MechanicBrick_Buttons;
    std::vector<std::pair<FString, UButton*>> Character_Buttons;

    std::vector<std::pair<FString, UButton*>> Color_Buttons;


    std::vector<std::pair<FString, UButton*>>* Current_Category_Buttons;



    std::vector<std::pair<BrickMenuCategory, UButton*>> Category_Buttons;

    UPROPERTY(meta = (BindWidget))
        UCanvasPanel* Panel_;

    UPROPERTY(meta = (BindWidget))
        UButton* Button_General_Category;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Weapon_Category;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Vehicle_Category;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Building_Category;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Mechanic_Category;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Character_Category;

    UPROPERTY(meta = (BindWidget))
        UScrollBox* box;


    //UFUNCTION()
    //void Button_2x1CarFenderClicked();

    BrickMenuCategory Current_Category;


    void removeWidget();
    void checkCategory();
    void setVisibilityCategoryButtons(const std::vector<std::pair<FString, UButton*>>& CategoryButtons, bool Visible);

public:
    virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;


};
