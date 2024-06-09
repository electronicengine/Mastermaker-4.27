// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include <vector>
#include <memory>
#include <map>

#include "GamePlayWidget.generated.h"

/**
 *
 */
UCLASS()
class MASTERMAKER_API UGamePlayWidget : public UUserWidget
{
    GENERATED_BODY()


public:

    UPROPERTY(meta = (BindWidget))
        UButton* Button_GetBrick;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Save;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Multiplayer;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Plug;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Interact;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Fire;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Punch;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Inventory;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Aim;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_LocationOffset;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_RotationOffset;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Jump;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Start;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Stop;
    UPROPERTY(meta = (BindWidget))
        UTextBlock* Text_Message;

    virtual bool Initialize();

    UFUNCTION()
        void Button_GetBrickClicked();
    UFUNCTION()
        void Button_SaveClicked();
    UFUNCTION()
        void Button_MultiplayerClicked();
    UFUNCTION()
        void Button_PlugClicked();
    UFUNCTION()
        void Button_InteractClicked();
    UFUNCTION()
        void Button_FireClicked();
    UFUNCTION()
        void Button_PunchClicked();
    UFUNCTION()
        void Button_InventoryClicked();
    UFUNCTION()
        void Button_AimClicked();
    UFUNCTION()
        void Button_ZoomInClicked();
    UFUNCTION()
        void Button_ZoomOutClicked();
    UFUNCTION()
        void Button_LocationOffsetClicked();
    UFUNCTION()
        void Button_RotationOffsetClicked();
    UFUNCTION()
        void Button_JumpClicked();
    UFUNCTION()
        void Button_StartClicked();
    UFUNCTION()
        void Button_StopClicked();



    void setInteractButtonVisibilty(ESlateVisibility Val);
    void setFireButtonVisibilty(ESlateVisibility Val);
    void setAimButtonVisibilty(ESlateVisibility Val);
    void setBuilderModeButtons();
    void setSlingModeButtons();
    void setCharacterModeButtons();
    void setMessage(const FString& Message, float TimeOut);


};

