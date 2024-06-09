// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBrick.h"
#include "../../character/UserCharacter.h"
#include "../../character/UserPlayerState.h"
#include "../../vehicle/VehicleBase.h"
#include "Kismet/GameplayStatics.h"
#include "../../MasterMakerGameInstance.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/EngineTypes.h"

AWeaponBrick::AWeaponBrick()
{
    PrimaryActorTick.bCanEverTick = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(Brick);
    static ConstructorHelpers::FObjectFinder<UParticleSystem> particle(TEXT("/Game/effects/effect"));
    Particle_Effect = particle.Object;


    Healt_ = 900000;
    Type_ = WeaponType::fire;
    Owned_ = false;
    Initialized_ = false;
}

void AWeaponBrick::notifyVehicleDetached()
{
    removeOwner();
}

void AWeaponBrick::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (Owner_Car) {
        if (Owner_Car->carHasPassenger() && Type_ == WeaponType::fire)
        {

            // Get the player controller

            // Get the player's camera rotation
            FRotator CameraRotation = Owner_Car->Camera->GetComponentRotation();

            // Set the actor's rotation to match the camera rotation
            SetActorRotation(CameraRotation);

        }
    }
    else if (Owner_Character) {
        if (Owner_Character->Builder_Mode_Enable && Type_ == WeaponType::fire)
        {

            // Get the player controller

            // Get the player's camera rotation
            FRotator CameraRotation = Owner_Character->Builder_Camera->GetComponentRotation();

            // Set the actor's rotation to match the camera rotation
            SetActorRotation(CameraRotation);

        }
    }

}

void AWeaponBrick::addedToVehicle(AVehicleBase* Vehicle)
{
    if (Type_ == WeaponType::fire) {
        enablePhysics(false);
        setCollisionProfile("OverlapAll");

    }

}

void AWeaponBrick::aimToRotation(FRotator& Rot)
{
    SetActorRotation(Rot);
}

void AWeaponBrick::removeOwner()
{
    if (Owner_Car)
        Owner_Car->removeWeaponToInventory(Weapon_Index);

    Owner_Car = nullptr;


    Brick->SetSimulatePhysics(true);
    Brick->SetCollisionProfileName(FName("BlockAll"));

    GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("BlockAll"));


}


void AWeaponBrick::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

    FVector crash_velocity = Brick->GetPhysicsLinearVelocity();
    int crash_speed = crash_velocity.Size();
    if (Cast<ABrick>(OtherActor)) {

        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle_Effect, GetActorLocation());
        Cast<ABrick>(OtherActor)->addDamage(crash_speed * 4, Hit.ImpactPoint, 0.01 * NormalImpulse);
    }
    else if (Cast<AVehicleBase>(OtherActor) && !(Cast<AVehicleBase>(OtherActor) == Owner_Car)) {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle_Effect, GetActorLocation());
        Cast<AVehicleBase>(OtherActor)->addDamage(crash_speed * 4, Hit.ImpactPoint, 0.01 * NormalImpulse);

    }
    else if (Cast<AEnemyCharacter>(OtherActor)) {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle_Effect, GetActorLocation());
        Cast<AEnemyCharacter>(OtherActor)->addDamage(crash_speed * 4, Hit.ImpactPoint, 0.001 * NormalImpulse);
    }


}


void AWeaponBrick::makePluginSettings()
{
    if (Item_Name.Find(MELEE_WEAPON_APPENDIX) >= 0)
        Type_ = WeaponType::melee;
    else if (Item_Name.Find(FIRE_WEAPON_APPENDIX) >= 0)
        Type_ = WeaponType::fire;
    else if (Item_Name.Find(EXPLOSIVE_WEAPON_APPENDIX) >= 0)
        Type_ = WeaponType::explosive;
    else if (Item_Name.Find(CLUSTER_WEAPON_APPENDIX) >= 0)
        Type_ = WeaponType::cluster;

    switch (Type_)
    {
    case WeaponType::fire:

        if (Owner_Car) {
            enablePhysics(false);
            setCollisionProfile("OverlapAll");
        }
        else {
            enablePhysics(true);
            setCollisionProfile("BlockAll");
            Brick->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
            Brick->SetNotifyRigidBodyCollision(true);
        }

        Barrel_Location = Brick->GetSocketLocation("barrel");
        Barrel_Rotation = Brick->GetSocketRotation("barrel");
        break;
    case WeaponType::melee:
        enablePhysics(true);
        setCollisionProfile("BlockAll");
        Brick->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
        Brick->SetNotifyRigidBodyCollision(true);
        if(!Initialized_)
            Brick->OnComponentHit.AddDynamic(this, &AWeaponBrick::OnHit);

        break;
    case WeaponType::explosive:
        enablePhysics(true);
        setCollisionProfile("BlockAll");
        Brick->SetMassOverrideInKg(NAME_None, 200, true);
        Brick->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
        Brick->SetNotifyRigidBodyCollision(true);
        SetLifeSpan(20);
        Mass_ = 200;
        if (!Initialized_)
            Brick->OnComponentHit.AddDynamic(this, &AWeaponBrick::OnHit);
        break;
    case WeaponType::cluster:
        Mass_ = 200;
        Brick->SetMassOverrideInKg(NAME_None, 200, true);
        Brick->SetNotifyRigidBodyCollision(true);

        makeCluster();
        break;
    default:
        break;
    }

    Initialized_ = true;

}

void AWeaponBrick::makeCluster()
{
}

void AWeaponBrick::applyForce(const FVector& ImpulseForce, const FVector& TorqueForce, const FVector& ForceLocation)
{


    if (Item_Name.Find(CLUSTER_WEAPON_APPENDIX) >= 0) {
        AWeaponBrick* bombs;
        FVector brick_loc = Brick->GetComponentLocation();
        Brick->DestroyComponent();

        bombs = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance())->spawnWeapon("Bomb_explosive_weapon", brick_loc + FVector(20, 0, 0), FRotator(0, 0, 0));
        bombs->scaleBomb(0.3);
        bombs->applyForce(ImpulseForce, TorqueForce, brick_loc);

        bombs = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance())->spawnWeapon("Bomb_explosive_weapon", brick_loc - FVector(20, 0, 0), FRotator(0, 0, 0));
        bombs->scaleBomb(0.3);
        bombs->applyForce(ImpulseForce, TorqueForce, brick_loc);

        bombs = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance())->spawnWeapon("Bomb_explosive_weapon", brick_loc + FVector(0, 20, 0), FRotator(0, 0, 0));
        bombs->scaleBomb(0.3);
        bombs->applyForce(ImpulseForce, TorqueForce, brick_loc);

        bombs = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance())->spawnWeapon("Bomb_explosive_weapon", brick_loc - FVector(0, 20, 0), FRotator(0, 0, 0));
        bombs->scaleBomb(0.3);
        bombs->applyForce(ImpulseForce, TorqueForce, brick_loc);

    }
    else {
        //Brick->AddImpulse(ImpulseForce);
        Brick->AddImpulseAtLocation(ImpulseForce, ForceLocation);
        // Apply the torque force for local rotation
        //Brick->AddTorqueInRadians(TorqueForce, NAME_None, true);
    }
}

void AWeaponBrick::scaleBomb(float Rate)
{
    Brick->SetWorldScale3D(FVector(Rate, Rate, Rate));
}


void AWeaponBrick::useWeapon(AUserCharacter* Character)
{
    Owner_Character = Character;

    //        controller->Possess(Cast<APawn>(this));



}

void AWeaponBrick::fire()
{

    if (Owner_Car && Type_ == WeaponType::fire) {
        FVector direction = Barrel_Rotation.Vector();

        Barrel_Location = Brick->GetSocketLocation("barrel");
        Barrel_Rotation = Brick->GetSocketRotation("barrel");
        APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
        Cast<AUserPlayerState>(controller->GetPawn()->GetPlayerState())->vehicleFire(Barrel_Location, direction);
    }

}


