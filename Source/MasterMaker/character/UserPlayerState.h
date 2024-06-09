// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../interface/ComposerInterface.h"
#include "../interface/PlugInterface.h"
#include "../interface/BreakableInterface.h"
#include "Kismet/GameplayStatics.h"
#include "UserPlayerState.generated.h"

class AUserCharacter;
/**
 *
 */
UCLASS()
class MASTERMAKER_API AUserPlayerState : public APlayerState
{
	GENERATED_BODY()

		AUserCharacter* _Char;
	static int Current_Id_Counter;
public:

	AUserPlayerState();

	template<typename T>
	T* findItemById(int BrickId) {
		// Iterate through all actors in the level and find the one with the specified name
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

		for (int i = 0; i < FoundActors.Num(); i++)
		{
			AActor* Actor = FoundActors[i];

			if (Cast<T>(Actor)) {
				int id = 0;
				if (Cast<IPlugInterface>(Actor))
					id = Cast<IPlugInterface>(Actor)->getId();
				if (Cast<IBreakableInterface>(Actor))
					id = Cast<IBreakableInterface>(Actor)->getDestroyedId();

				if (id == BrickId)
					return Cast<T>(Actor);
			}

		}

		// Actor with the specified name was not found
		return nullptr;
	}

	AActor* findPlugableAtLocation(FVector Location, float SearchRadius);
	int getUniq() { return ++Current_Id_Counter; };
	void fire();
	void buyBrick();
	void plugObject();
	void interact();
	void selectConstraits();
	void selectWeapon();
	void selectBrick();
	void equipVehicle();
	void equipBrick();
	void dropBrick();
	void vehicleFire(const FVector& Location, const FVector& Direction);
	void throwItem(int Id, const FVector& ForceVector);
	void destroyedBrick(int Id);
	void destroyedVehicle(int Id, const FVector& Loaction, const FRotator& Rotation);

	void load(const FString& Name, const FVector& Location);

	UFUNCTION(Server, Reliable, WithValidation)
		void fireOnServer(const FVector& Location, const FVector& ForwardVector);
	bool fireOnServer_Validate(const FVector& Location, const FVector& ForwardVector) { return true; }
	void fireOnServer_Implementation(const FVector& Location, const FVector& ForwardVector);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void fireOnMulticast(const FVector& Location, const FVector& ForwardVector);
	bool fireOnMulticast_Validate(const FVector& Location, const FVector& ForwardVector) { return true; }
	void fireOnMulticast_Implementation(const FVector& Location, const FVector& ForwardVector);

	UFUNCTION(Server, Reliable, WithValidation)
		void buyBrickOnServer(const FVector& Location, const FLinearColor& Color, const FString& Name);
	bool buyBrickOnServer_Validate(const FVector& Location, const FLinearColor& Color, const FString& Name) { return true; }
	void buyBrickOnServer_Implementation(const FVector& Location, const FLinearColor& Color, const FString& Name);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void buyBrickOnMulticast(const FVector& Location, const FLinearColor& Color, const FString& Name, int Id);
	bool buyBrickOnMulticast_Validate(const FVector& Location, const FLinearColor& Color, const FString& Name, int Id) { return true; }
	void buyBrickOnMulticast_Implementation(const FVector& Location, const FLinearColor& Color, const FString& Name, int Id);

	UFUNCTION(Server, Reliable, WithValidation)
		void plugObjectOnServer(const FVector& TargetPlugableLocation, int PluginIndex, int GrabbedId, int TargetId,
			const FRotator& TargetRotation, const FVector& TargetLocation, const FRotator& OffsetRotation, const FVector& OffsetLocation);
	bool plugObjectOnServer_Validate(const FVector& TargetPlugableLocation, int PluginIndex, int GrabbedId, int TargetId,
		const FRotator& TargetRotation, const FVector& TargetLocation, const FRotator& OffsetRotation, const FVector& OffsetLocation) {
		return true;
	}
	void plugObjectOnServer_Implementation(const FVector& TargetPlugableLocation, int PluginIndex, int GrabbedId, int TargetId,
		const FRotator& TargetRotation, const FVector& TargetLocation, const FRotator& OffsetRotation, const FVector& OffsetLocation);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void plugObjectOnMulticast(const FVector& TargetPlugableLocation, int PluginIndex, int GrabbedId, int TargetId,
			const FRotator& TargetRotation, const FVector& TargetLocation, const FRotator& OffsetRotation, const FVector& OffsetLocation);
	bool plugObjectOnMulticast_Validate(const FVector& TargetPlugableLocation, int PluginIndex, int GrabbedId, int TargetId,
		const FRotator& TargetRotation, const FVector& TargetLocation, const FRotator& OffsetRotation, const FVector& OffsetLocation) {
		return true;
	}
	void plugObjectOnMulticast_Implementation(const FVector& TargetPlugableLocation, int PluginIndex, int GrabbedId, int TargetId,
		const FRotator& TargetRotation, const FVector& TargetLocation, const FRotator& OffsetRotation, const FVector& OffsetLocation);

	UFUNCTION(Server, Reliable, WithValidation)
		void equipVehicleOnServer(int NearbyId);
	bool equipVehicleOnServer_Validate(int NearbyId) { return true; }
	void equipVehicleOnServer_Implementation(int NearbyId);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void equipVehicleOnMulticast(int NearbyId);
	bool equipVehicleOnMulticast_Validate(int NearbyId) { return true; }
	void equipVehicleOnMulticast_Implementation(int NearbyId);

	UFUNCTION(Server, Reliable, WithValidation)
		void exitVehicleOnServer();
	bool exitVehicleOnServer_Validate() { return true; }
	void exitVehicleOnServer_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
		void vehicleFireOnServer(const FVector& Location, const FVector& Direction);
	bool vehicleFireOnServer_Validate(const FVector& Location, const FVector& Direction) { return true; }
	void vehicleFireOnServer_Implementation(const FVector& Location, const FVector& Direction);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void vehicleFireOnMulticast(const FVector& Location, const FVector& Direction);
	bool vehicleFireOnMulticast_Validate(const FVector& Location, const FVector& Direction) { return true; }
	void vehicleFireOnMulticast_Implementation(const FVector& Location, const FVector& Direction);

	UFUNCTION(Server, Reliable, WithValidation)
		void destroyedBrickOnServer(int DestroyedId);
	bool destroyedBrickOnServer_Validate(int DestroyedId) { return true; }
	void destroyedBrickOnServer_Implementation(int DestroyedId);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void destroyedBrickOnMulticast(int DestroyedId);
	bool destroyedBrickOnMulticast_Validate(int DestroyedId) { return true; }
	void destroyedBrickOnMulticast_Implementation(int DestroyedId);

	UFUNCTION(Server, Reliable, WithValidation)
		void destroyedVehicleOnServer(int Id, const FVector& Location, const FRotator& Rotation);
	bool destroyedVehicleOnServer_Validate(int Id, const FVector& Location, const FRotator& Rotation) { return true; }
	void destroyedVehicleOnServer_Implementation(int Id, const FVector& Location, const FRotator& Rotation);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void destroyedVehicleOnMulticast(int Id, const FVector& Location, const FRotator& Rotation);
	bool destroyedVehicleOnMulticast_Validate(int Id, const FVector& Location, const FRotator& Rotation) { return true; }
	void destroyedVehicleOnMulticast_Implementation(int Id, const FVector& Location, const FRotator& Rotation);

	UFUNCTION(Server, Reliable, WithValidation)
		void equipBrickOnServer(int Id);
	bool equipBrickOnServer_Validate(int Id) { return true; }
	void equipBrickOnServer_Implementation(int Id);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void equipBrickOnMulticast(int Id);
	bool equipBrickOnMulticast_Validate(int Id) { return true; }
	void equipBrickOnMulticast_Implementation(int Id);

	UFUNCTION(Server, Reliable, WithValidation)
		void dropBrickOnServer(int Id);
	bool dropBrickOnServer_Validate(int Id) { return true; }
	void dropBrickOnServer_Implementation(int Id);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void dropBrickOnMulticast(int Id);
	bool dropBrickOnMulticast_Validate(int Id) { return true; }
	void dropBrickOnMulticast_Implementation(int Id);

	UFUNCTION(Server, Reliable, WithValidation)
		void hookCableOnServer(int ConstraitsId, int TargetId);
	bool hookCableOnServer_Validate(int ConstraitsId, int TargetId) { return true; }
	void hookCableOnServer_Implementation(int ConstraitsId, int TargetId);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void hookCableOnMulticast(int ConstraitsId, int TargetId);
	bool hookCableOnMulticast_Validate(int ConstraitsId, int TargetId) { return true; }
	void hookCableOnMulticast_Implementation(int ConstraitsId, int TargetId);

	UFUNCTION(Server, Reliable, WithValidation)
		void turnOnOffMachineOnServer(int ConstraitsId);
	bool turnOnOffMachineOnServer_Validate(int ConstraitsId) { return true; }
	void turnOnOffMachineOnServer_Implementation(int ConstraitsId);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void turnOnOffMachineOnMulticast(int ConstraitsId);
	bool turnOnOffMachineOnMulticast_Validate(int ConstraitsId) { return true; }
	void turnOnOffMachineOnMulticast_Implementation(int ConstraitsId);

	UFUNCTION(Server, Reliable, WithValidation)
		void loadOnServer(const FString& Info, const FVector& Location);
	bool loadOnServer_Validate(const FString& Info, const FVector& Location) { return true; }
	void loadOnServer_Implementation(const FString& Info, const FVector& Location);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void loadOnMulticast(const FString& Info, const FVector& Location, int StartId);
	bool loadOnMulticast_Validate(const FString& Info, const FVector& Location, int StartId) { return true; }
	void loadOnMulticast_Implementation(const FString& Info, const FVector& Location, int StartId);


	UFUNCTION(Server, Reliable, WithValidation)
		void throwItemOnServer(int ItemId, const FVector& ForceVector);
	bool throwItemOnServer_Validate(int ItemId, const FVector& ForceVector) { return true; }
	void throwItemOnServer_Implementation(int ItemId, const FVector& ForceVector);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void throwItemOnMulticast(int ItemId, const FVector& ForceVector);
	bool throwItemOnMulticast_Validate(int ItemId, const FVector& ForceVector) { return true; }
	void throwItemOnMulticast_Implementation(int ItemId, const FVector& ForceVector);


};
