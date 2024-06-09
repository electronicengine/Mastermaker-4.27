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

#include "MultiplayerWidget.generated.h"

/**
 *
 */
UCLASS()
class MASTERMAKER_API UMultiplayerWidget : public UUserWidget
{
    GENERATED_BODY()
        virtual bool Initialize();

    UPROPERTY(meta = (BindWidget))
        UButton* Button_Join;

    UPROPERTY(meta = (BindWidget))
        UButton* Button_Host;

    UPROPERTY(meta = (BindWidget))
        UEditableTextBox* ip_and_port;



public:

    UFUNCTION()
        void onClickedButton_Join();

    UFUNCTION()
        void onClickedButton_Host();

    UFUNCTION(BlueprintCallable, Category = "Removing")
        void removeWidget();


};
