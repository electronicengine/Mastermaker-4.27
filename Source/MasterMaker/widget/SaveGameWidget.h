// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"

#include <vector>
#include <memory>
#include <map>

#include "SaveGameWidget.generated.h"

class UMasterMakerGameInstance;


/**
 *
 */
UCLASS()
class MASTERMAKER_API USaveGameWidget : public UUserWidget
{
    GENERATED_BODY()
        virtual bool Initialize();

    UPROPERTY(meta = (BindWidget))
        UButton* Button_Save;

    UPROPERTY(meta = (BindWidget))
        UButton* Button_Load;

    UPROPERTY(meta = (BindWidget))
        UEditableTextBox* save_name;


    void removeWidget();

public:
    virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

    UFUNCTION()
        void onClickedButton_Save();

    UFUNCTION()
        void onClickedButton_Load();

    void CheckStoragePermission();

};
