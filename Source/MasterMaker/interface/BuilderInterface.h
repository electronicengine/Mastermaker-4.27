// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Camera/CameraComponent.h"
#include "PlugInterface.h"
#include "GameFramework/Character.h"
#include "UObject/Object.h"
#include "BuilderInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBuilderInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class MASTERMAKER_API IBuilderInterface
{
	GENERATED_BODY()


protected:

	UMaterial* Ghost_Possible_Material;
	UMaterial* Ghost_Imposible_Material;
	int Tick_Count;
	int Offset_Step;

	int Pivot_Width;
	ABrick* Ghost_Overlapped_Brick;



public:
	bool Building;
	FVector Aim_Impact_Point;
	bool Brick_Plugable;
	UStaticMeshComponent* Ghost;
	IPlugInterface* Target_Plugable_Item;
	AActor* Object_NearBy;
	FRotator OffSet_Rotation;
	FVector OffSet_Location;


	IBuilderInterface();
	void buildStart();
	void buildEnd();
	void grabObject(ABrick* Object);
	void dropObject(ABrick* Object);
	void plugObject(ABrick* GrabbedBrick, IPlugInterface* TargetPlugable, int PluginIndex,
		const FRotator& TargetRotation, const FVector& TargetLocation, const FRotator& OffsetRotation, const FVector& OffsetLocation);
	void giveOffsetRotation();
	void giveOffsetLocation();
	void lookForBuildingSpace(AActor* Target, FHitResult& OutHit);
	void checkGhostItemOverlap(AActor* OtherActor);
	class UMasterMakerGameInstance* Game_Instance;
	class ALegoFightGameStateBase* Game_State;
	class AUserPlayerState* Player_State;
	ABrick* Grabbable_Brick;
	bool Keeping_Bricks;
	bool Brick_Just_Plug;

};
