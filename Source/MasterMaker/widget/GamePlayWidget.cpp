// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayWidget.h"
#include "../character/UserCharacter.h"
#include "../character/UserPlayerState.h"

#include "../vehicle/VehicleBase.h"
#include "../level/MainForestTemplateScriptActor.h"


bool UGamePlayWidget::Initialize()
{
	Super::Initialize();

	Button_GetBrick->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_GetBrickClicked);
	Button_Save->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_SaveClicked);
	Button_Multiplayer->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_MultiplayerClicked);
	Button_Plug->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_PlugClicked);
	Button_Interact->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_InteractClicked);
	Button_Fire->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_FireClicked);
	Button_Punch->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_PunchClicked);
	Button_Inventory->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_InventoryClicked);
	Button_Aim->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_AimClicked);
	Button_LocationOffset->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_LocationOffsetClicked);
	Button_RotationOffset->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_RotationOffsetClicked);
	Button_Jump->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_JumpClicked);
	Button_Start->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_StartClicked);
	Button_Stop->OnClicked.AddDynamic(this, &UGamePlayWidget::Button_StopClicked);


	Button_Stop->SetVisibility(ESlateVisibility::Hidden);

	setCharacterModeButtons();

	return true;
}

void UGamePlayWidget::Button_GetBrickClicked()
{
	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());
	AUserPlayerState* player_state = Cast<AUserPlayerState>(character->GetPlayerState());
	if (character) {
		player_state->buyBrick();

		if (character->Builder_Mode_Enable) {
			setBuilderModeButtons();

			Button_GetBrick->SetVisibility(ESlateVisibility::Visible);
			Button_Interact->SetVisibility(ESlateVisibility::Visible);
			Button_Plug->SetVisibility(ESlateVisibility::Visible);
			Button_LocationOffset->SetVisibility(ESlateVisibility::Visible);
			Button_RotationOffset->SetVisibility(ESlateVisibility::Visible);

		}
		else if (character->Sling_Mode_Enable) {
			setSlingModeButtons();
			Button_GetBrick->SetVisibility(ESlateVisibility::Visible);

		}
		else {
			setCharacterModeButtons();
		}

	}
}

void UGamePlayWidget::Button_SaveClicked()
{
	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());
	if (character) {
		character->saveAndLoad();
	}
}

void UGamePlayWidget::Button_MultiplayerClicked()
{
	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());
	if (character) {
		character->host();
	}
}

void UGamePlayWidget::Button_PlugClicked()
{
	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());
	AUserPlayerState* player_state = Cast<AUserPlayerState>(character->GetPlayerState());

	if (character) {
		player_state->plugObject();

		if (character->Builder_Mode_Enable) {
			setBuilderModeButtons();

			Button_GetBrick->SetVisibility(ESlateVisibility::Visible);
			Button_Plug->SetVisibility(ESlateVisibility::Hidden);

		}


	}
}

void UGamePlayWidget::Button_InteractClicked()
{

	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());


	if (character) {
		AUserPlayerState* player_state = Cast<AUserPlayerState>(character->GetPlayerState());

		player_state->interact();
		if (character->Keeping_Bricks) {
			Button_Plug->SetVisibility(ESlateVisibility::Visible);
		}
		else {
			Button_Plug->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else {

		AVehicleBase* vehicle = Cast<AVehicleBase>(GetOwningPlayerPawn());
		AUserPlayerState* player_state = Cast<AUserPlayerState>(vehicle->GetPlayerState());

		if (player_state) {
			player_state->interact();
		}
	}



}

void UGamePlayWidget::Button_FireClicked()
{
	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());

	if (character) {
		AUserPlayerState* player_state = Cast<AUserPlayerState>(character->GetPlayerState());
		player_state->fire();
	}
	else {
		AVehicleBase* vehicle = Cast<AVehicleBase>(GetOwningPlayerPawn());

		if (vehicle)
			vehicle->fire();
	}
}

void UGamePlayWidget::Button_PunchClicked()
{
	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());

	if (character) {
		character->punch();
	}
}

void UGamePlayWidget::Button_InventoryClicked()
{
	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());
	if (character) {
		character->openInventoryWidget();

		if (character->Builder_Mode_Enable) {
			setBuilderModeButtons();
			Button_GetBrick->SetVisibility(ESlateVisibility::Visible);
		}
		else if (character->Sling_Mode_Enable) {
			setSlingModeButtons();
			Button_GetBrick->SetVisibility(ESlateVisibility::Visible);
		}

	}

}

void UGamePlayWidget::Button_AimClicked()
{

	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());
	if (character) {
		character->changeCameraMode();

		if (character->Builder_Mode_Enable) {
			setBuilderModeButtons();
		}
		else if (character->Sling_Mode_Enable) {

			setSlingModeButtons();

		}
		else {
			setCharacterModeButtons();

		}
	}

}

void UGamePlayWidget::Button_ZoomInClicked()
{
	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());
	if (character) {
		character->zoom(5);
	}
}

void UGamePlayWidget::Button_ZoomOutClicked()
{
	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());
	if (character) {
		character->zoom(-5);
	}
}

void UGamePlayWidget::Button_LocationOffsetClicked()
{
	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());
	if (character) {
		character->giveOffsetLocation();
	}
}

void UGamePlayWidget::Button_RotationOffsetClicked()
{
	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());
	if (character) {
		character->giveOffsetRotation();
	}
}

void UGamePlayWidget::Button_JumpClicked()
{
	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());
	if (character) {
		character->Jump();
	}

	AVehicleBase* vehicle = Cast<AVehicleBase>(GetOwningPlayerPawn());
	if (vehicle)
		vehicle->jump();

}

void UGamePlayWidget::Button_StartClicked()
{
	Button_Start->SetVisibility(ESlateVisibility::Hidden);

	FTimerHandle timer;
	GetWorld()->GetTimerManager().SetTimer(timer, [this]()
		{
			Button_Stop->SetVisibility(ESlateVisibility::Visible);
			#ifdef DESKTOP_BUILD
				Button_Stop->SetVisibility(ESlateVisibility::Hidden);
			#endif
		}, 4, false);

	AMainForestTemplateScriptActor* level = Cast<AMainForestTemplateScriptActor>(GetWorld()->GetLevelScriptActor());
	level->startGame();

}

void UGamePlayWidget::Button_StopClicked()
{
	FTimerHandle timer;
	GetWorld()->GetTimerManager().SetTimer(timer, [this]()
		{
			Button_Start->SetVisibility(ESlateVisibility::Visible);
			#ifdef DESKTOP_BUILD
				Button_Start->SetVisibility(ESlateVisibility::Hidden);
			#endif

		}, 4, false);
	Button_Stop->SetVisibility(ESlateVisibility::Hidden);

	AMainForestTemplateScriptActor* level = Cast<AMainForestTemplateScriptActor>(GetWorld()->GetLevelScriptActor());
	level->stopGame();
}

void UGamePlayWidget::setInteractButtonVisibilty(ESlateVisibility Val)
{
	Button_Interact->SetVisibility(Val);
#ifdef DESKTOP_BUILD
	Button_Interact->SetVisibility(ESlateVisibility::Hidden);
#endif
}

void UGamePlayWidget::setFireButtonVisibilty(ESlateVisibility Val)
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("setFireButtonVisibilty"));

	Button_Fire->SetVisibility(Val);
#ifdef DESKTOP_BUILD
	Button_Fire->SetVisibility(ESlateVisibility::Hidden);
#endif
}

void UGamePlayWidget::setAimButtonVisibilty(ESlateVisibility Val)
{
	Button_Aim->SetVisibility(Val);

#ifdef DESKTOP_BUILD
	Button_Aim->SetVisibility(ESlateVisibility::Hidden);
#endif
}

void UGamePlayWidget::setBuilderModeButtons()
{
	AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());

	Button_GetBrick->SetVisibility(ESlateVisibility::Hidden);
	Button_Save->SetVisibility(ESlateVisibility::Visible);

	if (character->Keeping_Bricks) {
		Button_Plug->SetVisibility(ESlateVisibility::Visible);
		Button_LocationOffset->SetVisibility(ESlateVisibility::Visible);
		Button_RotationOffset->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		Button_Plug->SetVisibility(ESlateVisibility::Hidden);
		Button_LocationOffset->SetVisibility(ESlateVisibility::Hidden);
		Button_RotationOffset->SetVisibility(ESlateVisibility::Hidden);
	}

	Button_Interact->SetVisibility(ESlateVisibility::Hidden);
	Button_Fire->SetVisibility(ESlateVisibility::Visible);
	Button_Inventory->SetVisibility(ESlateVisibility::Visible);
	Button_Aim->SetVisibility(ESlateVisibility::Visible);
	Button_Jump->SetVisibility(ESlateVisibility::Hidden);
	Button_Punch->SetVisibility(ESlateVisibility::Hidden);

#ifdef DESKTOP_BUILD
	Button_GetBrick->SetVisibility(ESlateVisibility::Hidden);
	Button_Save->SetVisibility(ESlateVisibility::Hidden);
	Button_Plug->SetVisibility(ESlateVisibility::Hidden);
	Button_Interact->SetVisibility(ESlateVisibility::Hidden);
	Button_Fire->SetVisibility(ESlateVisibility::Hidden);
	Button_Punch->SetVisibility(ESlateVisibility::Hidden);
	Button_Inventory->SetVisibility(ESlateVisibility::Hidden);
	Button_Aim->SetVisibility(ESlateVisibility::Hidden);
	Button_LocationOffset->SetVisibility(ESlateVisibility::Hidden);
	Button_RotationOffset->SetVisibility(ESlateVisibility::Hidden);
	Button_Jump->SetVisibility(ESlateVisibility::Hidden);
	Text_Message->SetVisibility(ESlateVisibility::Hidden);
#endif // DESKTOP_BUILD


}

void UGamePlayWidget::setSlingModeButtons()
{
	Button_GetBrick->SetVisibility(ESlateVisibility::Hidden);
	Button_Save->SetVisibility(ESlateVisibility::Hidden);
	Button_Plug->SetVisibility(ESlateVisibility::Hidden);
	Button_LocationOffset->SetVisibility(ESlateVisibility::Hidden);
	Button_RotationOffset->SetVisibility(ESlateVisibility::Hidden);
	Button_Interact->SetVisibility(ESlateVisibility::Hidden);
	Button_Fire->SetVisibility(ESlateVisibility::Hidden);
	Button_Punch->SetVisibility(ESlateVisibility::Hidden);
	Button_Inventory->SetVisibility(ESlateVisibility::Visible);
	Button_Aim->SetVisibility(ESlateVisibility::Visible);
	Button_Jump->SetVisibility(ESlateVisibility::Hidden);

#ifdef DESKTOP_BUILD
	Button_GetBrick->SetVisibility(ESlateVisibility::Hidden);
	Button_Save->SetVisibility(ESlateVisibility::Hidden);
	Button_Plug->SetVisibility(ESlateVisibility::Hidden);
	Button_Interact->SetVisibility(ESlateVisibility::Hidden);
	Button_Fire->SetVisibility(ESlateVisibility::Hidden);
	Button_Punch->SetVisibility(ESlateVisibility::Hidden);
	Button_Inventory->SetVisibility(ESlateVisibility::Hidden);
	Button_Aim->SetVisibility(ESlateVisibility::Hidden);
	Button_LocationOffset->SetVisibility(ESlateVisibility::Hidden);
	Button_RotationOffset->SetVisibility(ESlateVisibility::Hidden);
	Button_Jump->SetVisibility(ESlateVisibility::Hidden);
	Text_Message->SetVisibility(ESlateVisibility::Hidden);
#endif // DESKTOP_BUILD


}

void UGamePlayWidget::setCharacterModeButtons() {

	Button_GetBrick->SetVisibility(ESlateVisibility::Hidden);
	Button_Save->SetVisibility(ESlateVisibility::Hidden);
	Button_Plug->SetVisibility(ESlateVisibility::Hidden);
	Button_Interact->SetVisibility(ESlateVisibility::Hidden);
	Button_Fire->SetVisibility(ESlateVisibility::Hidden);
	Button_Punch->SetVisibility(ESlateVisibility::Visible);
	Button_Inventory->SetVisibility(ESlateVisibility::Hidden);
	Button_Aim->SetVisibility(ESlateVisibility::Visible);
	Button_LocationOffset->SetVisibility(ESlateVisibility::Hidden);
	Button_RotationOffset->SetVisibility(ESlateVisibility::Hidden);
	Button_Jump->SetVisibility(ESlateVisibility::Visible);
	Text_Message->SetVisibility(ESlateVisibility::Hidden);

#ifdef DESKTOP_BUILD
	Button_GetBrick->SetVisibility(ESlateVisibility::Hidden);
	Button_Save->SetVisibility(ESlateVisibility::Hidden);
	Button_Plug->SetVisibility(ESlateVisibility::Hidden);
	Button_Interact->SetVisibility(ESlateVisibility::Hidden);
	Button_Fire->SetVisibility(ESlateVisibility::Hidden);
	Button_Punch->SetVisibility(ESlateVisibility::Hidden);
	Button_Inventory->SetVisibility(ESlateVisibility::Hidden);
	Button_Aim->SetVisibility(ESlateVisibility::Hidden);
	Button_LocationOffset->SetVisibility(ESlateVisibility::Hidden);
	Button_RotationOffset->SetVisibility(ESlateVisibility::Hidden);
	Button_Jump->SetVisibility(ESlateVisibility::Hidden);
	Text_Message->SetVisibility(ESlateVisibility::Hidden);
#endif // DESKTOP_BUILD


}

void UGamePlayWidget::setMessage(const FString& Message, float TimeOut)
{
	Text_Message->SetText(FText::FromString(Message));
	Text_Message->SetVisibility(ESlateVisibility::Visible);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			Text_Message->SetVisibility(ESlateVisibility::Hidden);

		}, TimeOut, false);

}

