// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Brick.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Net/UnrealNetwork.h"
#include "WeaponBrick.generated.h"



enum WeaponType {
    fire,
    melee,
    explosive,
    cluster
};

class AUserCharacter;
class ACharacter;
/**
 *
 */
UCLASS()
class MASTERMAKER_API AWeaponBrick : public ABrick
{
    GENERATED_BODY()


        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCameraComponent* Camera;
    UParticleSystem* Particle_Effect;
    bool Initialized_;
    USceneComponent* barrel;
    ACharacter* User_;
    int Last_Time;
    bool Owned_;
public:
    AUserCharacter* Owner_Character;
    WeaponType Type_;
    FVector Barrel_Location;
    FRotator Barrel_Rotation;

    AWeaponBrick();

    void useWeapon(AUserCharacter* Character);

    void fire();

    void addedToVehicle(AVehicleBase* Vehicle);
    void aimToRotation(FRotator& Rotation);
    void removeOwner();
    void makePluginSettings();
    void makeCluster();
    void applyForce(const FVector& ImpulseForce, const FVector& TorqueForce, const FVector& ForceLocation);
    void scaleBomb(float Rate);
    FVector getBarrelLocation() { return Brick->GetSocketLocation("barrel"); }
    FVector getBarrelDirection() { return  Brick->GetSocketRotation("barrel").Vector(); }
    void notifyVehicleDetached();

    virtual void Tick(float DeltaTime) override;
    UFUNCTION()
        void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
