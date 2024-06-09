// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyVehicle.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "EnemyVehicleAIController.h"
#include "../level/MainForestTemplateScriptActor.h"


AEnemyVehicle::AEnemyVehicle()
{
    CrashedTo_Target = false;

    Team_Id = 0;

    Overlap_Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Overlap_Box"));
    Overlap_Box->SetupAttachment(RootComponent);
    Overlap_Box->SetCollisionProfileName(TEXT("OverlapAll"));
    Overlap_Box->SetGenerateOverlapEvents(true);
    Overlap_Box->SetWorldScale3D(FVector(11.0, 9.33, 1.0));

    Overlap_Box->OnComponentBeginOverlap.AddDynamic(this, &AEnemyVehicle::OnDelegateOverlap);
    //static ConstructorHelpers::FObjectFinder<UBlueprint> AICon(TEXT("Blueprint'/Game/bricks/test_ai.test_ai'"));
    //AIControllerClass = AICon.Object->GeneratedClass;

    AIControllerClass = AEnemyVehicleAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}



void AEnemyVehicle::BeginPlay()
{
    Super::BeginPlay();

    Cast<AMainForestTemplateScriptActor>(GetWorld()->GetLevelScriptActor())->enemySpawned(this);


}

void AEnemyVehicle::GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const
{
    Location = GetMesh()->GetSocketLocation("EYE");
    Rotation = GetMesh()->GetSocketRotation("EYE");
}





void AEnemyVehicle::OnDelegateOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    AVehicleBase* user_car = Cast<AVehicleBase>(OtherActor);

    if (user_car != nullptr)
    {
        if (user_car->Team_Id == 1) {
            CrashedTo_Target = true;
        }
    }
}


void AEnemyVehicle::fireToEnemy(FRotator& ToEnemyRot)
{

    static int last = 0;
    int now = time(NULL);

    static int weapon_index;

    if (now - last >= 1) {

        if (Weapons.size() != 0)
        {

            for (std::pair<int, AWeaponBrick*> weapon : Weapons) {
                if (!weapon.second->IsActorBeingDestroyed()) {
                    weapon.second->aimToRotation(ToEnemyRot);
                    weapon.second->fire();
                }
            }
        }

        last = now;

    }
}

