// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../interface/BreakableInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class MASTERMAKER_API AEnemyCharacter : public ACharacter, public IBreakableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();
	UAnimMontage* Punch_Animation;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void punch(AActor* PunchtoActor);

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);


};
