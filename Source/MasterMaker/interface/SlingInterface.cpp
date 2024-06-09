// Fill out your copyright notice in the Description page of Project Settings.


#include "SlingInterface.h"
#include "../brick/Brick.h"
#include "../brick/weapon/WeaponBrick.h"
#include "BuilderInterface.h"
// Include necessary UE4 headers
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "../character/UserCharacter.h"
#include "../character/UserPlayerState.h"
// Add default functionality here for any ISlingInterface functions that are not pure virtual.

ISlingInterface::ISlingInterface()
{
    Sling_Mode_Enable = false;
    Stretching_Sling = false;
}



void ISlingInterface::throwTheItem(const FVector& ForceVector, AActor* Item)
{
    if (Cast<ABrick>(Item)) {

        float Mass = Cast<ABrick>(Item)->Mass_; // Get the mass of the projectile
        FVector InitialVelocityVector = ForceVector * (500 / Mass); // Calculate the initial velocity vector

        FVector CurrentVelocity = FVector(0, 0, 0); // Get the current velocity

        FVector DeltaVelocity = InitialVelocityVector - CurrentVelocity; // Calculate the change in velocity

        FVector ImpulseForce = Mass * DeltaVelocity; // Calculate the impulse force

        // Apply the impulse force to the projectile

        Cast<IBuilderInterface>(this)->dropObject(Cast<ABrick>(Item));

        FVector TorqueForce = FVector(0.0f, 1.0f, 0);  // Adjust the torque force as needed

        Cast<AWeaponBrick>(Item)->applyForce(ImpulseForce, TorqueForce, Cast<AWeaponBrick>(Item)->GetActorLocation());
    }
}

void ISlingInterface::strechTheSling() {

    FVector2D MousePosition;
    Projectile_Item = Cast<AUserCharacter>(this)->Grabbable_Brick;

    if (Viewport_Client && Projectile_Item) {
        Viewport_Client->GetMousePosition(MousePosition);


        if (Stretching_Sling) {
            Stretching_Sling = false;
            Stretch_End = FVector(MousePosition.X, MousePosition.Y, 0);
            Cast<AUserCharacter>(this)->Sling_Bed->SetWorldLocation(Projectile_Location);

            Cast<AUserPlayerState>(Cast<AUserCharacter>(this)->GetPlayerState())->throwItem(Cast<IPlugInterface>(Projectile_Item)->Id_, Throw_Force);
            Projectile_Item = nullptr;
        }
        else {
            Stretching_Sling = true;
            Stretch_Begin = FVector(MousePosition.X, MousePosition.Y, 0);
        }
    }


}

// ...

