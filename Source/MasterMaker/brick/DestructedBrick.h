// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <vector>
#include <map>
#include "EngineUtils.h"
#include <string>
#include "DestructedBrick.generated.h"

UCLASS()
class MASTERMAKER_API ADestructedBrick : public AActor
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
		USceneComponent* Default_Root;

public:
	// Sets default values for this actor's properties
	ADestructedBrick();
	UMaterial* Brick_Material;
	UMaterial* CubeMan_Skin_Material;
	UMaterial* CubeMan_Shirt_Material;
	UMaterial* CubeMan_Pant_Material;
	UMaterial* CubeMan_Hair_Material;
	FString Name_;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	TArray<UStaticMeshComponent*> Pieces;
	std::map<int, std::string> Mesh_Paths;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void setColor(FVector ColorVec);
	void setMesh(FString Name);
	void setImpactPointAndVelocity(const FVector& ImpactPoint, const FVector& Velocity);
	void setBrickMesh(const TArray<UStaticMesh*>& Meshes);
	void setCubeManCharacterMesh(const TArray<UStaticMesh*>& Meshes);
	void setVehicleMesh(const TArray<UStaticMesh*>& Meshes);

};
