// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include <functional>
#include <map>
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include <functional>
#include <vector>
#include <iostream>
#include <list>
#include <queue>
#include "Animation/AnimMontage.h"
#include "EnemyCharacterAIController.generated.h"

#define WAYPOINT_INTERVAL   1000
class AUserCharacter;
/**
 *
 */
UCLASS()
class MASTERMAKER_API AEnemyCharacterAIController : public AAIController
{
    GENERATED_BODY()



        void setupPerception();

    FVector Target_Location;
    AActor* Target_;
    FVector Next_Move_Location;

    bool Target_Reached;
    bool WayPoint_Reached;

    APawn* OwnerChar;
    UAnimationAsset* Punch_Animation;

    std::queue<FVector> Way_Points;

    FTimerHandle Punch_Timer_Handle;

protected:
    virtual void OnPossess(APawn* InPawn) override;

public:

    AEnemyCharacterAIController();

    virtual void BeginPlay() override;


    virtual void Tick(float DeltaSeconds) override;

    void punchToEnemy();
    FVector getRandomPoint(const FVector& Center, float Radius);
    bool IsPointReached(const FVector& Point, const TArray<FVector>& ReachedPoints);
    std::queue<FVector> calculateWayPoints(float Distance, float Radius);
    int moveOnWayPoints(std::queue<FVector>& Way_Points);


    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

    virtual void Destroyed() override;

};
