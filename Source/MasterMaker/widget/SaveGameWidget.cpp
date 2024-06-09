// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGameWidget.h"
#include "Engine/Engine.h"
#include "../character/UserCharacter.h"
#include "../character/UserPlayerState.h"
#include "../vehicle/VehicleBase.h"
#include "../MasterMakerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "RendererInterface.h"
#include "DrawDebugHelpers.h"
#include "../MasterMakerGameInstance.h"
#include "Engine/World.h"
#include "Components/Image.h"
#include "Slate/SlateBrushAsset.h"
#include "Styling/SlateBrush.h"
#include "SlateBasics.h"
#include "Widgets/Images/SImage.h"
//#include "Android/AndroidApplication.h"
//#include "Android/AndroidJNI.h"
//#include "Android/AndroidPermissionFunctionLibrary.h"

#include "Runtime/Engine/Classes/Engine/Texture2D.h"

#include "../brick/Brick.h"


bool USaveGameWidget::Initialize()
{

    Super::Initialize();
    // Button_1x1Triangle->OnClicked.AddDynamic(this, &USandBoxWidget::Button_1x1TriangleClicked);
    Button_Save->OnClicked.AddDynamic(this, &USaveGameWidget::onClickedButton_Save);
    Button_Load->OnClicked.AddDynamic(this, &USaveGameWidget::onClickedButton_Load);

    return true;
}

void USaveGameWidget::removeWidget()
{

    SetVisibility(ESlateVisibility::Collapsed);
    UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());


    game_instance->loadCharacterPanel();
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    controller->SetInputMode(FInputModeGameOnly());
    controller->bShowMouseCursor = false;

}

void USaveGameWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
}

void USaveGameWidget::onClickedButton_Save()
{

    FText txt = save_name->GetText();
    UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());

    game_instance->saveGame(txt.ToString());

    removeWidget();

}

void USaveGameWidget::onClickedButton_Load()
{
    FText txt = save_name->GetText();
    UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());

    game_instance->loadGame(txt.ToString());

    removeWidget();
}


void USaveGameWidget::CheckStoragePermission()
{
    // Check if READ_EXTERNAL_STORAGE permission is granted
    //JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    //if (Env)
    //{
    //    jclass ClassID = FAndroidApplication::FindJavaClass("com/epicgames/ue4/GameActivity");
    //    jmethodID MethodID = FJavaWrapper::FindMethod(Env, ClassID, "checkStoragePermission", "()Z", false);

    //    if (MethodID)
    //    {
    //        bool bHasPermission = FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, MethodID);

    //        if (bHasPermission)
    //        {
    //            // Storage permission is granted
    //            UE_LOG(LogTemp, Warning, TEXT("Storage permission granted"));
    //        }
    //        else
    //        {
    //            // Storage permission is not granted
    //            UE_LOG(LogTemp, Warning, TEXT("Storage permission not granted"));
    //        }
    //    }
    //}
}