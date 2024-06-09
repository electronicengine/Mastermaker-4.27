// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerWidget.h"
#include "Engine/Engine.h"
#include "../character/UserCharacter.h"
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
#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "Online.h"
#include "../brick/Brick.h"

bool UMultiplayerWidget::Initialize()
{
	Super::Initialize();
	// Button_1x1Triangle->OnClicked.AddDynamic(this, &USandBoxWidget::Button_1x1TriangleClicked);
	Button_Join->OnClicked.AddDynamic(this, &UMultiplayerWidget::onClickedButton_Join);
	Button_Host->OnClicked.AddDynamic(this, &UMultiplayerWidget::onClickedButton_Host);

	return true;
}

void UMultiplayerWidget::removeWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
	UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());
	RemoveFromParent();
	game_instance->GamePlay_Panel->RemoveFromParent();

	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	controller->SetInputMode(FInputModeGameOnly());
	controller->bShowMouseCursor = false;

}

void UMultiplayerWidget::onClickedButton_Join()
{
	// Execute console command
	//if (GEngine)
	//{
	//    GEngine->Exec(nullptr, TEXT("open 127.0.0.1:7777"));
	//}
}

void UMultiplayerWidget::onClickedButton_Host()
{
	//bool bIsLAN = true;
	//bool bIsPresence = false;
	//int MaxNumPlayers = 5;

	//TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	//// Get the Online Subsystem to work with
	//IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	//if (OnlineSub)
	//{
	//	// Get the Session Interface, so we can call the "CreateSession" function on it
	//	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

	//	if (Sessions.IsValid())
	//	{
	//		/*
	//			Fill in all the Session Settings that we want to use.

	//			There are more with SessionSettings.Set(...);
	//			For example the Map or the GameMode/Type.
	//		*/
	//		SessionSettings = MakeShareable(new FOnlineSessionSettings());

	//		SessionSettings->bIsLANMatch = bIsLAN;
	//		SessionSettings->bUsesPresence = bIsPresence;
	//		SessionSettings->NumPublicConnections = MaxNumPlayers;
	//		SessionSettings->NumPrivateConnections = 0;
	//		SessionSettings->bAllowInvites = true;
	//		SessionSettings->bAllowJoinInProgress = true;
	//		SessionSettings->bShouldAdvertise = true;
	//		SessionSettings->bAllowJoinViaPresence = true;
	//		SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

	//		SessionSettings->Set(SETTING_MAPNAME, FString("NewMap1"), EOnlineDataAdvertisementType::DontAdvertise);

	//		// Set the delegate to the Handle of the SessionInterface

	//		// Our delegate should get called when this is complete (doesn't need to be successful!)
	//		Sessions->CreateSession(5, "multises", *SessionSettings);
	//	}
	//}


}
