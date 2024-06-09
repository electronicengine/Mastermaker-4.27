// Fill out your copyright notice in the Description page of Project Settings.


#include "UserCharacter.h"
#include "Engine/Engine.h"
#include "../vehicle/VehicleBase.h"
#include "../MasterMakerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "RendererInterface.h"
#include "DrawDebugHelpers.h"
#include "../level/MasterMakerSaveGame.h"
#include "Json.h"
#include "../widget/GamePlayWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "UserPlayerState.h"
#include "Kismet/KismetMathLibrary.h"

AUserCharacter::AUserCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    setupMesh();


    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
    SpringArm->SetupAttachment(RootComponent);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->SetRelativeLocation(FVector(-165.196976, 0, 192.280701));
    Camera->SetRelativeRotation(FRotator(0, -10.0, 0));

    Builder_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Aim_Camera"));
    Builder_Camera->SetupAttachment(GetMesh());
    Builder_Camera->SetRelativeLocation(FVector(-66.029541, -134.599854, 759.365051));
    Builder_Camera->SetRelativeRotation(FRotator(0.000003, -20.000061, 90.000221));
    Builder_Camera->bUsePawnControlRotation = true;

    Sling_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Sling_Camera"));
    Sling_Camera->SetupAttachment(GetMesh());
    Sling_Camera->SetRelativeLocation(FVector(-66.029541, -134.599854, 759.365051));
    Sling_Camera->SetRelativeRotation(FRotator(0.000003, -20.000061, 90.000221));

    Barrel_ = CreateDefaultSubobject<USceneComponent>(TEXT("Barrel"));
    Barrel_->SetupAttachment(GetMesh());

    Punch_Component = CreateDefaultSubobject<UBoxComponent>(TEXT("Punch"));
    Punch_Component->SetupAttachment(GetMesh());
    Punch_Component->SetSimulatePhysics(false);
    Punch_Component->SetNotifyRigidBodyCollision(true);
    Punch_Component->SetCollisionProfileName(FName("BlockAll"));
    Punch_Component->OnComponentHit.AddDynamic(this, &AUserCharacter::OnPunchHit);

    Interaction_Component = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Interact"));
    Interaction_Component->SetupAttachment(RootComponent);
    Interaction_Component->SetCollisionProfileName(TEXT("OverlapAll"));

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);


    Ghost_Item = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ghost"));
    //Ghost_Item->SetupAttachment(RootComponent);
    Ghost_Item->SetSimulatePhysics(false);
    Ghost_Item->SetCollisionProfileName("OverlapAll");

    Punch_Animation = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/characters/cube_man/animations/cube_man_Punch_Montage.cube_man_Punch_Montage"));

    base_turn_rate = 0.6f;
    look_up_rate = 0.6f;
    Ghost = Ghost_Item;
    Camera_Mode = CameraMode::normal;
    Punching = false;
    Last_Touch = 0;
}



// Sets default values
void AUserCharacter::setupMesh()
{
    //static ConstructorHelpers::FObjectFinder<USkeletalMesh>
    //    mesh_asset(TEXT("SkeletalMesh'/Game/characters/lego_man/skeleton/lego_man.lego_man'"));
    //USkeletalMesh* mesh = mesh_asset.Object;
    //GetMesh()->SetSkeletalMesh(mesh);

    //static ConstructorHelpers::FObjectFinder<UMaterial>
    //    material_asset0(TEXT("Material'/Game/characters/materials/Head.Head'"));
    //UMaterial* head_material = material_asset0.Object;
    //GetMesh()->SetMaterial(0, head_material);

    //static ConstructorHelpers::FObjectFinder<UMaterial>
    //    material_asset1(TEXT("Material'/Game/characters/materials/UnderPants.UnderPants'"));
    //UMaterial* underpants_material = material_asset1.Object;
    //GetMesh()->SetMaterial(1, underpants_material);

    //static ConstructorHelpers::FObjectFinder<UMaterial>
    //    material_asset3(TEXT("Material'/Game/characters/materials/Arms.Arms'"));
    //UMaterial* arm_material = material_asset3.Object;
    //GetMesh()->SetMaterial(3, arm_material);

    //static ConstructorHelpers::FObjectFinder<UMaterial>
    //    material_asset4(TEXT("Material'/Game/characters/materials/Waists.Waists'"));
    //UMaterial* waist_material = material_asset4.Object;
    //GetMesh()->SetMaterial(4, waist_material);

    //static ConstructorHelpers::FObjectFinder<UMaterial>
    //    material_asset5(TEXT("Material'/Game/characters/materials/Legs.Legs'"));
    //UMaterial* leg_material = material_asset5.Object;
    //GetMesh()->SetMaterial(5, leg_material);

    static ConstructorHelpers::FObjectFinder<UMaterial>
        material_asset2(TEXT("Material'/Game/bricks/materials/brick_material.brick_material'"));
    UMaterial* hand_material = material_asset2.Object;
    GetMesh()->SetMaterial(2, hand_material);

    static ConstructorHelpers::FObjectFinder<UMaterial>
        ghost_asset(TEXT("Material'/Game/bricks/materials/ghost_possible.ghost_possible'"));
    Ghost_Possible_Material = ghost_asset.Object;

    static ConstructorHelpers::FObjectFinder<UMaterial>
        ghost_impossible_asset(TEXT("Material'/Game/bricks/materials/ghost_impossible.ghost_impossible'"));
    Ghost_Imposible_Material = ghost_impossible_asset.Object;

    //static ConstructorHelpers::FClassFinder<ADestructedBrick>
    //         bullet(TEXT("Class'/Script/LegoFight.Bullet_C'"));
    Bullet_Container = ABullet::StaticClass();

    //GetMesh()->SetRelativeScale3D(FVector(0.5225, 0.5225, 0.5225));
}


// Called when the game starts or when spawned
void AUserCharacter::BeginPlay()
{
    Super::BeginPlay();

    Game_Instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());


    if (Game_Instance->GamePlay_Panel) { Game_Instance->GamePlay_Panel = nullptr; }
    if (Game_Instance->SaveGame_Panel) { Game_Instance->SaveGame_Panel = nullptr; }
    if (Game_Instance->SandBox_Panel) { Game_Instance->SandBox_Panel = nullptr; }

    Game_Instance->loadCharacterPanel();

    Ghost_Item->OnComponentBeginOverlap.AddDynamic(this, &AUserCharacter::OnGhostOverLap);
    Viewport_Client = GetWorld()->GetGameViewport();
  

    Interaction_Component->OnComponentBeginOverlap.AddDynamic(this, &AUserCharacter::OnInteractBegin);
    Interaction_Component->OnComponentEndOverlap.AddDynamic(this, &AUserCharacter::OnInteractEnd);
    Punch_Component->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("punch_socket"));
    GetMesh()->SetCollisionProfileName("OverlapAll");
    GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Overlap);

}


void AUserCharacter::calculateProjectilePath(AActor* Projectile, const FVector& ForwardVector)
{
    // Variables declaration
    FHitResult HitResult;
    TArray<FVector> PathPositions;
    FVector LastTraceDestination;
    float k = 0;

    // Calculate k based on the type of Projectile (assuming it's a Brick)
    if (Cast<ABrick>(Projectile))
        k = 480 / Cast<ABrick>(Projectile)->Mass_;

    // Get start location and calculate launch velocity
    FVector StartLocation = Projectile->GetActorLocation();
    FVector LaunchVelocity = ForwardVector * k;
    float ProjectileRadius = 5;

    // Settings for projectile path prediction
    const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        TArray<AActor*> ActorsToIgnore;

    bool bHit = UGameplayStatics::Blueprint_PredictProjectilePath_ByObjectType(
        this,
        HitResult,
        PathPositions,
        LastTraceDestination,
        StartLocation,
        LaunchVelocity,
        true,  // bTracePath
        ProjectileRadius,
        ObjectTypes,
        true,  // bTraceComplex
        ActorsToIgnore,
        EDrawDebugTrace::ForOneFrame,  // DrawDebugType
        1.0f,  // DrawDebugTime
        10.0f,  // SimFrequency
        1.0f,  // MaxSimTime
        -980.0f  // OverrideGravityZ (adjust as needed, -980.0f is default gravity)
    );
}

void AUserCharacter::join()
{
}

void AUserCharacter::host()
{
    Game_Instance->loadMultiplayerPanel();
}

void AUserCharacter::punch()
{
    if (!Builder_Mode_Enable && !Sling_Mode_Enable && !Punching) {

        GetCharacterMovement()->DisableMovement();
        PlayAnimMontage(Punch_Animation, 1.0f, FName("Punch"));
        Punching = true;
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
            {
                GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
                Punching = false;
            }, 1, false);
    }

}

void AUserCharacter::openSlingMode(AMechanicBrick* Sling, UStaticMeshComponent* Bed, const FVector& SceneLocation, const FRotator& SceneRotation)
{
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    Camera->SetActive(false);
    Builder_Camera->SetActive(false);
    Sling_Camera->SetActive(true);
    FVector direction = Sling->GetActorForwardVector();

    // Calculate the new position in front of the SceneLocation

    Sling_Camera->SetWorldLocation(SceneLocation - direction * 400);

    Sling_Camera->SetWorldRotation(SceneRotation);
    controller->bShowMouseCursor = true;
    Builder_Mode_Enable = false;
    Sling_Mode_Enable = true;
    Projectile_Location = SceneLocation;
    Projectile_Rotation = SceneRotation;
    Sling_Bed = Bed;

    buildEnd();

}



void AUserCharacter::OnPunchHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (Cast<AEnemyCharacter>(OtherActor)) {
        Cast<AEnemyCharacter>(OtherActor)->addDamage(20, Hit.ImpactPoint, NormalImpulse);
    }
    else if (Cast<ABrick>(OtherActor)) {
        // Cast<ABrick>(OtherActor)->addDamage(20, Hit.ImpactPoint, NormalImpulse);
    }
}




void AUserCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


    if (Builder_Mode_Enable == true)
    {
        FHitResult OutHit;
        FVector Start = Builder_Camera->GetComponentLocation();
        FVector forward_vector = Builder_Camera->GetComponentRotation().Vector();
        FVector End = Start + (forward_vector * 3000.0f);
        FCollisionQueryParams CollisionParams;

        GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);
        Aim_Impact_Point = OutHit.ImpactPoint;

        lookForBuildingSpace(OutHit.GetActor(), OutHit);
    }
    else if (Sling_Mode_Enable && Stretching_Sling) {
        FVector forward;
        if (Sling_Bed && Grabbable_Brick) {

            Sling_Bed->SetWorldLocation(Grabbable_Brick->GetActorLocation() - FVector(50, 0, 0));
            Sling_Bed->SetWorldRotation(Grabbable_Brick->GetActorRotation());

        }
        UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();

        FVector2D MousePosition;
        ViewportClient->GetMousePosition(MousePosition);


        FVector cam_rot_vec = Sling_Camera->GetComponentRotation().Vector();

        float x = (Stretch_Begin.Y - MousePosition.Y) * -cam_rot_vec.X * 2;
        float y = ((Stretch_Begin.X - MousePosition.X) + (Stretch_Begin.Y - MousePosition.Y));
        float z = (Stretch_Begin.Y - MousePosition.Y) * -1;
        Throw_Force = FVector(x, y, z);
        if (Grabbable_Brick) {
            Grabbable_Brick->SetActorLocation(Projectile_Location - Throw_Force * 0.2);

            calculateProjectilePath(Grabbable_Brick, Throw_Force);

        }
    }

}


void AUserCharacter::OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location)
{
    double CurrentTime = FPlatformTime::Seconds() * 1000.0;
    double TimeSinceLastTouch = CurrentTime - Last_Touch;

    if (Sling_Mode_Enable) {
        Stretch_Begin = Location;
        Stretching_Sling = true;
        if (!Sling_Bed && !Grabbable_Brick) {
            Sling_Bed->AttachToComponent(Cast<USceneComponent>(Grabbable_Brick), FAttachmentTransformRules(EAttachmentRule::SnapToTarget,
                EAttachmentRule::SnapToTarget,
                EAttachmentRule::KeepWorld, false));
        }
    }
    else {
#ifndef DESKTOP_BUILD

        if (TimeSinceLastTouch < 200 && Last_Touch != 0)
        {
            if(Builder_Mode_Enable)
                Player_State->fire();
        }

        Initial_Touch_Location = Location;
        Last_Touch = CurrentTime;
#endif
        Initial_Touch_Location = Location;
    }

}


void AUserCharacter::OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location)
{

    if (Sling_Mode_Enable) {
        Stretch_End = Location;
        Stretching_Sling = false;

        throwTheItem(Throw_Force, Grabbable_Brick);
        Projectile_Item = nullptr;
    }
    else {
        Initial_Touch_Location = FVector::ZeroVector;
    }

}

void AUserCharacter::TouchMoved(ETouchIndex::Type FingerIndex, FVector Location)
{
    if (Sling_Mode_Enable) {
        Stretch_End = Location;
        Stretching_Sling = true;
    }
    else {

#ifndef DESKTOP_BUILD

        FVector Delta = Location - Initial_Touch_Location;
        float Sensitivity = 0.1f;

        float YawChange = Delta.X * Sensitivity;
        float PitchChange = Delta.Y * Sensitivity;

        if (YawChange > 15 || PitchChange > 15)
            return;

        AddControllerYawInput(YawChange);
        AddControllerPitchInput(PitchChange);
        Initial_Touch_Location = Location;

#endif // DESKTOP_BUILD
        
    }

}


void AUserCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

    Super::SetupPlayerInputComponent(PlayerInputComponent);

    Player_State = Cast<AUserPlayerState>(GetPlayerState());

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAxis("MoveForward", this, &AUserCharacter::moveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AUserCharacter::moveRight);

    PlayerInputComponent->BindAxis("Turn", this, &AUserCharacter::turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AUserCharacter::lookUp);
    PlayerInputComponent->BindAxis("Zoom", this, &AUserCharacter::zoom);

    PlayerInputComponent->BindAction("Fire", IE_Pressed, Player_State, &AUserPlayerState::fire);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ISlingInterface::strechTheSling);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AUserCharacter::punch);

    PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AUserCharacter::changeCameraMode);
    //PlayerInputComponent->BindAction("Aim", IE_Released, this, &AUserCharacter::aimEnd);

    PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &IInteractInterface::openInventoryWidget);
    PlayerInputComponent->BindAction("Equip", IE_Pressed, Player_State, &AUserPlayerState::interact);
    PlayerInputComponent->BindAction("OffSetItem", IE_Pressed, this, &IBuilderInterface::giveOffsetLocation);
    PlayerInputComponent->BindAction("TurnObject", IE_Pressed, this, &IBuilderInterface::giveOffsetRotation);

    PlayerInputComponent->BindAction("Plug", IE_Pressed, Player_State, &AUserPlayerState::plugObject);
    PlayerInputComponent->BindAction("Buy", IE_Pressed, Player_State, &AUserPlayerState::buyBrick);
    PlayerInputComponent->BindAction("Save", IE_Pressed, this, &IInteractInterface::saveAndLoad);
    PlayerInputComponent->BindAction("Load", IE_Pressed, this, &AUserCharacter::saveAndLoad);
    PlayerInputComponent->BindAction("join", IE_Pressed, this, &AUserCharacter::join);
    PlayerInputComponent->BindAction("host", IE_Pressed, this, &AUserCharacter::host);

    PlayerInputComponent->BindTouch(IE_Pressed, this, &AUserCharacter::OnTouchPressed);
    PlayerInputComponent->BindTouch(IE_Released, this, &AUserCharacter::OnTouchReleased);
    PlayerInputComponent->BindTouch(IE_Repeat, this, &AUserCharacter::TouchMoved);


}



void AUserCharacter::OnInteractBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    if (Builder_Mode_Enable == false && Cast<IPlugInterface>(OtherActor) && !Cast<AUserCharacter>(OtherActor)) {
        interactNearby(OtherActor);
        Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setInteractButtonVisibilty(ESlateVisibility::Visible);
    }

}

void AUserCharacter::OnInteractEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    //if(!Grabbable_Brick)
        //Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setInteractButtonVisibilty(ESlateVisibility::Hidden);

}

void AUserCharacter::OnGhostOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    checkGhostItemOverlap(OtherActor);
}


void AUserCharacter::moveForward(float Value)
{
    if ((Controller) && Value != 0.0f)
    {
        if (Sling_Mode_Enable) {

            Sling_Camera->AddLocalOffset(FVector(0, 0, Value * 10));
        }
        else if (Builder_Mode_Enable) {
            Builder_Camera->AddLocalOffset(FVector(Value * 10, 0, 0));
        }
        else {
            const FVector direction = FRotationMatrix(FRotator(0, Controller->GetControlRotation().Yaw, 0)).GetUnitAxis(EAxis::X);
            AddMovementInput(direction, Value);
        }
    }
}



void AUserCharacter::moveRight(float Value)
{
    if ((Controller) && Value != 0.0f)
    {
        if (Sling_Mode_Enable) {

            Sling_Camera->AddLocalOffset(FVector(0, Value * 10, 0));
        }
        else if (Builder_Mode_Enable) {

            Builder_Camera->AddLocalOffset(FVector(0, Value * 10, 0));
        }
        else {
            const FVector direction = FRotationMatrix(FRotator(0, Controller->GetControlRotation().Yaw, 0)).GetUnitAxis(EAxis::Y);
            AddMovementInput(direction, Value);
        }
    }
}



void AUserCharacter::turn(float Value)
{
    AddControllerYawInput(Value);
}



void AUserCharacter::lookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AUserCharacter::zoom(float Value)
{

    if (Builder_Mode_Enable) {
        Builder_Camera->AddLocalOffset(FVector(0, 0, Value * 10));
    }
    else if (Sling_Mode_Enable) {
        Sling_Camera->AddLocalOffset(FVector(Value * 10, 0, 0));
    }

}



void AUserCharacter::fire(const FVector& Location, const FVector& ForwardVector)
{
    ABullet* bullet_ptr = GetWorld()->SpawnActor<ABullet>(Bullet_Container, Location, FRotator(0, 0, 0));

    if (bullet_ptr != nullptr) {
        bullet_ptr->Strenght_ = 40;
        bullet_ptr->setMassAndStrenght(3, 100);
        bullet_ptr->addFireImpulse(ForwardVector, 30000);

    }

}



void AUserCharacter::changeCameraMode()
{
    Camera_Mode = (CameraMode)((int)Camera_Mode + 1);
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    switch (Camera_Mode)
    {
    case CameraMode::normal:
        Camera->SetActive(true);
        Builder_Camera->SetActive(false);
        Sling_Camera->SetActive(false);
        controller->bShowMouseCursor = false;

        Builder_Mode_Enable = false;
        Sling_Mode_Enable = false;
        if (Target_Usable_Weapon)
            Target_Usable_Weapon->Owner_Character = nullptr;
        Target_Usable_Weapon->Owner_Character = nullptr;

        buildEnd();
        break;

    case CameraMode::builder:
        Camera->SetActive(false);
        Builder_Camera->SetActive(true);
        Sling_Camera->SetActive(false);
        controller->bShowMouseCursor = false;

        Builder_Mode_Enable = true;
        Sling_Mode_Enable = false;
        if (Target_Usable_Weapon)
            Target_Usable_Weapon->Owner_Character = nullptr;

        Target_Usable_Weapon = nullptr;

        buildStart();

        break;

    default:
        Camera_Mode = CameraMode::normal;

        Camera->SetActive(true);
        Builder_Camera->SetActive(false);
        Sling_Camera->SetActive(false);

        Builder_Mode_Enable = false;
        Sling_Mode_Enable = false;
        controller->bShowMouseCursor = false;

        if (Target_Usable_Weapon)
            Target_Usable_Weapon->Owner_Character = nullptr;
        Target_Usable_Weapon = nullptr;

        buildEnd();

        break;
    }


}



void AUserCharacter::aimEnd()
{
    Camera->SetActive(true);
    Builder_Camera->SetActive(false);

    Builder_Mode_Enable = false;

}












