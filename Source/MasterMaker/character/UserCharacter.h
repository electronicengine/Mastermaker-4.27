// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../brick/Brick.h"
#include "../brick/weapon/Bullet.h"
#include "Materials/Material.h"
#include "../interface/BuilderInterface.h"
#include "../interface/InteractInterface.h"
#include "../interface/SlingInterface.h"
#include "../interface/BreakableInterface.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "UserCharacter.generated.h"

#define COLLISION_PROJECTILE ECC_GameTraceChannel1
#define COLLISION_CHARACTER ECC_GameTraceChannel2

//#define DESKTOP_BUILD 1

enum CameraMode {
    normal,
    builder,
    sling
};

class AVehicleBase;
class AWeaponBrick;

UCLASS()
class MASTERMAKER_API AUserCharacter : public ACharacter, public IInteractInterface, public ISlingInterface
{
    GENERATED_BODY()


        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        USpringArmComponent* SpringArm;



    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCapsuleComponent* Interaction_Component;



    UPROPERTY(VisibleAnywhere)
        UStaticMeshComponent* Ghost_Item;



    UPROPERTY(EditAnywhere, Category = "Barrel")
        USceneComponent* Barrel_;

    AWeaponBrick* Interactable_Weapon;

    CameraMode Camera_Mode;
    float Last_Touch;

    bool Punching;
    void setupMesh();

public:
    // Sets default values for this character's properties
    AUserCharacter();

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        bool Is_In_Car;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCameraComponent* Builder_Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCameraComponent* Sling_Camera;

    /** Projectile class to spawn */
    UPROPERTY(EditDefaultsOnly, Category = "Bullet")
        TSubclassOf<class ABullet> Bullet_Container;

    UPROPERTY(EditAnywhere, Category = "Control")
        float base_turn_rate;

    UPROPERTY(EditAnywhere, Category = "Control")
        float look_up_rate;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UBoxComponent* Punch_Component;

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


    UFUNCTION()
        void OnInteractBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
        void OnInteractEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
        void OnGhostOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
        void OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location);

    UFUNCTION()
        void OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location);

    UFUNCTION()
        void TouchMoved(ETouchIndex::Type FingerIndex, FVector Location);

    void moveForward(float Value);
    void moveRight(float Value);
    void turn(float Value);
    void lookUp(float Value);
    void zoom(float Value);
    void fire(const FVector& Location, const FVector& ForwardVector);
    void changeCameraMode();
    void aimEnd();
    void calculateProjectilePath(AActor* Projectile, const FVector& ForwardVector);
    void join();
    void host();
    void punch();

    FConstructionInfo Construction_Info;
    UStaticMeshComponent* Sling_Bed;
    AWeaponBrick* Target_Usable_Weapon;
    UAnimMontage* Punch_Animation;
    FVector Initial_Touch_Location;

    UFUNCTION(BlueprintCallable)
        void openSlingMode(AMechanicBrick* Sling, UStaticMeshComponent* Bed, const FVector& SceneLocation, const FRotator& SceneRotation);


    UFUNCTION()
        void OnPunchHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

};
