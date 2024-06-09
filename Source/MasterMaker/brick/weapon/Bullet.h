// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Bullet.generated.h"

UCLASS()
class MASTERMAKER_API ABullet : public AActor
{
	GENERATED_BODY()
		UPrimitiveComponent* Primitive;

	bool First_Hit;
public:
	// Sets default values for this actor's properties
	ABullet();

	UPROPERTY(EditAnywhere, Category = "BulletMesh")
		UStaticMeshComponent* Bullet_Mesh;

	TSubclassOf<class UParticleSystem> Particle;
	UParticleSystem* Particle_Effect;

	float Strenght_;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void addFireImpulse(FVector Direction, float Strenght);
	void spawnparticle();
	void setMassAndStrenght(float Strenght, float Mass);
private:

	// FUNCTIONS

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

};
