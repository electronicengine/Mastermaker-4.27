// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "../brick/weapon/Bullet.h"
#include "../brick/Brick.h"
#include "EnemyCharacterAIController.h"
#include "../level/MainForestTemplateScriptActor.h"
// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Healt_ = 1;

	Punch_Animation = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/characters/cube_man/animations/cube_man_Punch_Montage.cube_man_Punch_Montage"));
	Destroyed_Id = 0;
	Item_Name = "CubeManEnemyCharacter";

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->SetNotifyRigidBodyCollision(true);
	GetMesh()->SetCollisionProfileName(FName("BlockAll"));
	GetMesh()->OnComponentHit.AddDynamic(this, &AEnemyCharacter::OnHit);
	Cast<AMainForestTemplateScriptActor>(GetWorld()->GetLevelScriptActor())->enemySpawned(this);
}


// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	AEnemyCharacterAIController* AIController = Cast<AEnemyCharacterAIController>(GetController());

	if (AIController)
	{
		if (Cast<ABrick>(OtherActor)) {
			AIController->StopMovement();
			punch(OtherActor);

		}
	}

}


void AEnemyCharacter::punch(AActor* PunchtoActor)
{

	GetCharacterMovement()->DisableMovement();
	PlayAnimMontage(Punch_Animation, 1.0f, FName("Punch"));

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, PunchtoActor]()
		{
			if (Cast<ABrick>(PunchtoActor)) {
				Cast<ABrick>(PunchtoActor)->addDamage(3, GetActorLocation(), GetActorForwardVector() * 100);
			}
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		}, 1, false);


}
