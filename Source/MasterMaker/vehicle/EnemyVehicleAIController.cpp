// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyVehicleAIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Math/Vector.h"
#include "Engine/Engine.h"
#include "VehicleBase.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "EnemyVehicle.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "EnemyVehicleAIController.h"
#include "NavigationPath.h"

AEnemyVehicleAIController::AEnemyVehicleAIController()
{
    PrimaryActorTick.bCanEverTick = true;

    Enemy_Found = false;
    Current_Task_Index = 0;
    Last_TurnedDown_Time = 0;
    Jobs.push_back(std::bind(&AEnemyVehicleAIController::crashToEnemy, this));
}




void AEnemyVehicleAIController::OnPossess(APawn *InPawn)
{
    OwnerChar = InPawn;
    SetPawn(InPawn);
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("OnPossess"));
    Super::OnPossess(InPawn);
}



void AEnemyVehicleAIController::Tick(float DeltaSeconds)
{


    TargetVehicle = Cast<AVehicleBase>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (TargetVehicle) {
        Enemy_Found = true;

        Enemy_Location = TargetVehicle->GetActorLocation();

        if (TargetVehicle->carHasPassenger())
            attackToUser();
    }

    //Cast<AVehicleBase>(GetPawn())->checkVehicleTurnedDown();
}

void AEnemyVehicleAIController::Destroyed()
{
    GetWorld()->GetTimerManager().ClearTimer(Attack_Timer);
}



bool AEnemyVehicleAIController::crashToEnemy()
{
    FVector TargetLocation = Enemy_Location;
    FVector CurrentLocation = GetPawn()->GetActorLocation();
    AEnemyVehicle* vehicle = Cast<AEnemyVehicle>(GetPawn());

    DrawDebugLine(GetWorld(), CurrentLocation, TargetLocation, FColor::Cyan, false, -1, 0, 1);
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSystem)
    {
        // Calculate a path from the vehicle's current location to the destination
        FNavLocation StartLocation;
        FNavLocation EndLocation;

        if (NavSystem->ProjectPointToNavigation(GetPawn()->GetActorLocation(), StartLocation, FVector::ZeroVector))
        {
            if (NavSystem->ProjectPointToNavigation(TargetLocation, EndLocation, FVector::ZeroVector))
            {
                UNavigationPath* NavigationPath = NavSystem->FindPathToLocationSynchronously(GetWorld(), StartLocation.Location, EndLocation.Location);

                if (NavigationPath && NavigationPath->IsValid())
                {
                    TArray<FVector> ReachedPoints;

                    if (NavigationPath->PathPoints.Num() >= 2) {
                        DrawDebugSphere(GetWorld(), NavigationPath->PathPoints[1], 10, 32, FColor::Red, false, -1, 0, 1);
                        moveTo(NavigationPath->PathPoints[1]);
                    }

                }
            }
        }
    }

    
         if(vehicle->CrashedTo_Target){
             vehicle->CrashedTo_Target = false;
             GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString("Crashed"));

             return true;
         }else{
             return false;
         }


}


int AEnemyVehicleAIController::attackToUser()
{
    if (GetPawn()) {
        AEnemyVehicle* Vehicle = Cast<AEnemyVehicle>(GetPawn());
        FVector CurrentLocation = Vehicle->GetActorLocation();

        FVector TargetLocation = Enemy_Location;

        FRotator lookup = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, (TargetLocation - FVector(0, 0, 100.0f)));
        if(Vehicle)
            Vehicle->fireToEnemy(lookup);

        if (Jobs.size() >= 1) {
            std::list<std::function<bool()>>::iterator i = Jobs.begin();

            bool ret = (*i)();

        }
    }

    return 0;
}

bool AEnemyVehicleAIController::IsPointReached(const FVector& Point, const TArray<FVector>& ReachedPoints)
{
    // Check if the given Point is already in the ReachedPoints array
    return ReachedPoints.ContainsByPredicate([Point](const FVector& ReachedPoint) {
        return FVector::DistSquared(Point, ReachedPoint) < FMath::Square(100.0f); // Adjust the proximity threshold as needed
        });
}

bool AEnemyVehicleAIController::moveTo(const FVector& Target)
{
    AEnemyVehicle* Vehicle = Cast<AEnemyVehicle>(GetPawn());

    FVector DesiredDirection = (Target - GetPawn()->GetActorLocation()).GetSafeNormal();


    FRotator error_rot = (UKismetMathLibrary::FindLookAtRotation(Vehicle->GetActorLocation(), Target) - Vehicle->GetActorRotation());

    //GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::SanitizeFloat(error_rot.Yaw));

    if (error_rot.Yaw > 70 || error_rot.Yaw < -70)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString("back"));
        Vehicle->GetVehicleMovementComponent()->SetHandbrakeInput(false);

        Vehicle->moveForward(-1.0f);
        Vehicle->moveRight(-1.0f);

    }
    else {
        //GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString("front"));

        //// Calculate throttle based on the desired direction (you may need to fine-tune this)
        float ThrottleValue = FVector::DotProduct(GetPawn()->GetActorForwardVector(), DesiredDirection);
        float MaxThrottle = 1.0f; // Adjust as needed
        float MinThrottle = 0.0f;
        float Throttle = FMath::Clamp(ThrottleValue, MinThrottle, MaxThrottle);

        //// Calculate steering based on the desired direction (you may need to fine-tune this)
        float SteeringValue = FVector::CrossProduct(GetPawn()->GetActorForwardVector(), DesiredDirection).Z;
        float MaxSteering = 1.0f; // Adjust as needed
        float MinSteering = -1.0f;
        float Steering = FMath::Clamp(SteeringValue, MinSteering, MaxSteering);

        //// Apply throttle and steering to control the vehicle
        //// Replace AYourVehicle with your actual vehicle class
        //Vehicle->moveLocation(Target);

        Vehicle->moveForward(MaxThrottle);
        Vehicle->moveRight(Steering);
        
    }

  

  

    return true;
}


int AEnemyVehicleAIController::goRandomLocation()
{
    return 0;
}



int AEnemyVehicleAIController::turnArround()
{
    return 0;
}



void AEnemyVehicleAIController::BeginPlay()
{


    Super::BeginPlay();

   
}



//void AEnemyVehicleAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
//{
//
//    int enemy_number_nearby = 0;
//
//    for(int i = 0; i < UpdatedActors.Num(); i++)
//    {
//        TargetVehicle = Cast<AVehicleBase>(UpdatedActors[i]);
//
//
//        if(TargetVehicle != nullptr)
//        {
//
//            if(TargetVehicle->Team_Id == 1)
//            {
//                enemy_number_nearby = 1;
//                Enemy_Found = true;
//                Enemy_Location = TargetVehicle->GetActorLocation();
//
//                if(TargetVehicle->carHasPassenger())
//                    attackToUser();
//                
//            }
//
//        }
//    }
//
//    if(enemy_number_nearby == 0)
//        Enemy_Found = false;

//}


