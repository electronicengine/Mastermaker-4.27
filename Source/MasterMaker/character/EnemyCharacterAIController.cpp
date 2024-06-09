// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacterAIController.h"
#include "UserCharacter.h"
#include "Navigation/PathFollowingComponent.h" // For FPathFollowingResult
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Math/Vector.h"
#include "Engine/Engine.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "NavigationPath.h"
#include "EnemyCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"

AEnemyCharacterAIController::AEnemyCharacterAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	setupPerception();



	Punch_Animation = LoadObject<UAnimationAsset>(nullptr, TEXT("/Game/characters/cube_man/animations/cube_man_Punch.cube_man_Punch"));
}


void AEnemyCharacterAIController::BeginPlay()
{
	Super::BeginPlay();
	Target_Reached = true;
	WayPoint_Reached = true;
}

void AEnemyCharacterAIController::setupPerception()
{
	//SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	//SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

	//SightConfig->SightRadius = AISightRadius;
	//SightConfig->LoseSightRadius = AILoseSightRadius;
	//SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView;
	//SightConfig->SetMaxAge(AISightAge);

	//SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	//SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	//SightConfig->DetectionByAffiliation.bDetectNeutrals = true;


	//GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	//GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AEnemyCharacterAIController::OnPerceptionUpdated);
	//GetPerceptionComponent()->ConfigureSense(*SightConfig);
}

void AEnemyCharacterAIController::OnPossess(APawn* InPawn)
{
	//Super::Possess(InPawn);
	OwnerChar = InPawn;
	SetPawn(InPawn);
	Super::OnPossess(InPawn);
}


void AEnemyCharacterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Target_ = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (Target_) {
		Target_Location = Target_->GetActorLocation();
		FVector char_location = GetPawn()->GetActorLocation();

		float diff = (char_location - Target_Location).Size();
		if (diff >= 100) {  //target not reached

			if (Target_Reached) {
				GetWorld()->GetTimerManager().ClearTimer(Punch_Timer_Handle);

				float distance = (char_location - Target_Location).Size();
				Way_Points = calculateWayPoints(distance, 1000);
				Target_Reached = false;

			}
			else {

				moveOnWayPoints(Way_Points);
			}
		}
		else {  // Target Reached
			Target_Reached = true;
		}
	}





}

void AEnemyCharacterAIController::punchToEnemy()
{

	GetWorld()->GetTimerManager().SetTimer(Punch_Timer_Handle, [this]()
		{
			FVector character_location;
			APawn* pwn = GetPawn();

			if (pwn) {
				character_location = pwn->GetActorLocation();

				FVector direction = Target_Location - character_location;
				direction.Normalize();
				FRotator NewRotation = UKismetMathLibrary::MakeRotFromX(direction);

				pwn->SetActorRotation(NewRotation);
				Cast<AEnemyCharacter>(GetPawn())->punch(Target_);
			}



		}, 1, true);

}


FVector AEnemyCharacterAIController::getRandomPoint(const FVector& Center, float Radius)
{
	FVector RandomDirection = FMath::VRand();
	float RandomDistance = FMath::RandRange(0.0f, Radius);
	FVector RandomLocation = Center + (RandomDistance * RandomDirection);

	return RandomLocation;
}


bool AEnemyCharacterAIController::IsPointReached(const FVector& Point, const TArray<FVector>& ReachedPoints)
{
	return false;
}

std::queue<FVector> AEnemyCharacterAIController::calculateWayPoints(float Distance, float Radius)
{
	std::queue<FVector> way_points;

	int number = Distance / WAYPOINT_INTERVAL;

	for (int i = 0; i < number; i++) {
		way_points.push(getRandomPoint(FVector(0, 0, 0), 1000));
	}

	return way_points;
}

int AEnemyCharacterAIController::moveOnWayPoints(std::queue<FVector>& WayPoints)
{

	if (WayPoint_Reached) {
		if (WayPoints.size() <= 0) {
			Next_Move_Location = Target_Location;
		}
		else {
			FVector offset_location = Way_Points.front();
			WayPoints.pop();
			Next_Move_Location = FVector(offset_location.X + Target_Location.X, offset_location.Y + Target_Location.Y, Target_Location.Z);
		}

		WayPoint_Reached = false;

	}
	else {
		if ((Next_Move_Location - GetPawn()->GetActorLocation()).Size() <= 100) {
			Next_Move_Location = Target_Location;
		}

		MoveToLocation(Next_Move_Location, 45);

	}




	return 0;
}




void AEnemyCharacterAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	// Call the base class implementation
	Super::OnMoveCompleted(RequestID, Result);

	if (Result.IsSuccess())
	{
		WayPoint_Reached = true;
		if (Way_Points.size() <= 0) {
			Target_Reached = true;
			punchToEnemy();
		}
	}


}

void AEnemyCharacterAIController::Destroyed()
{
	GetWorld()->GetTimerManager().ClearTimer(Punch_Timer_Handle);

}
