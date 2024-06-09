// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include <map>
#include <iostream>
#include "BreakableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBreakableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MASTERMAKER_API IBreakableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	float Healt_;
	bool Breaked_;
	int Destroyed_Id;
	FString Item_Name;
	FVector Item_Color;

	IBreakableInterface();

	bool addDamage(float Value, const FVector& ImpactPoint, const FVector& Velocity);
	void breakItem(const FVector& ImpactPoint, const FVector& Velocity);
	void setId(int Id);
	int getDestroyedId() { return Destroyed_Id; }
	void addDamageToPlugins(float Value, const FVector& ImpactPoint, const FVector& Velocity);
	bool addDamageToSelf(float Value, const FVector& ImpactPoint, const FVector& Velocity);

};
