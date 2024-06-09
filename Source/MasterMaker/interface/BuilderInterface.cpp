// Fill out your copyright notice in the Description page of Project Settings.


#include "BuilderInterface.h"
#include "../brick/Brick.h"
#include "Kismet/GameplayStatics.h"
#include "../brick/MechanicBrick.h"
#include "Engine/World.h"
#include "../widget/GamePlayWidget.h"
#include "../MasterMakerGameInstance.h"
#include "../character/UserPlayerState.h"

IBuilderInterface::IBuilderInterface()
{



    Brick_Plugable = true;
    Brick_Just_Plug = false;
    Keeping_Bricks = false;
    Building = false;
    OffSet_Rotation.Roll = 0;
    OffSet_Rotation.Yaw = 0;
    OffSet_Rotation.Pitch = 0;
    OffSet_Location = FVector(0, 0, 0);
    Offset_Step = 0;

}

// Add default functionality here for any IBuilderInterface functions that are not pure virtual.
void IBuilderInterface::buildStart()
{
    Building = true;

}

void IBuilderInterface::buildEnd()
{

    Building = false;
    Ghost->SetVisibility(false);
    Target_Plugable_Item = nullptr;
    Brick_Just_Plug = false;
    Tick_Count = 0;


}

void IBuilderInterface::grabObject(ABrick* Object)
{

    if (Object && Cast<ACharacter>(this))
    {

        Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setInteractButtonVisibilty(ESlateVisibility::Visible);
        Object->enablePhysics(false);
        Object->setCollisionProfile("OverlapAll");


        Object->SetActorLocation(Cast<AUserCharacter>(this)->GetMesh()->GetSocketLocation(FName("grab_socket")),
            false, nullptr, ETeleportType::ResetPhysics);

        Object->AttachToComponent(Cast<AUserCharacter>(this)->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget,
            EAttachmentRule::SnapToTarget,
            EAttachmentRule::KeepWorld, false), TEXT("grab_socket"));

        Ghost->SetStaticMesh(Object->getBrickMesh());
        Keeping_Bricks = true;

        FVector Origin;
        FVector BoxExtent;
        float SphereRadius;

        UKismetSystemLibrary::GetComponentBounds(Ghost, Origin, BoxExtent, SphereRadius);

        if (BoxExtent.Y >= BoxExtent.X)
            Pivot_Width = (BoxExtent.Y / 12.5);
        else
            Pivot_Width = (BoxExtent.X / 12.5);

        Object->Grabbed = true;

        if (Object->Owner_Item) {
            Object->notifyToOwnerItemDestroyed(Object->Item_Index);
        }
    }

}



void IBuilderInterface::dropObject(ABrick* Object)
{

    if (Object != nullptr)
    {
        const FDetachmentTransformRules& attachment_rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
            EDetachmentRule::KeepWorld,
            EDetachmentRule::KeepWorld, true);

        Object->DetachFromActor(attachment_rules);
        Object->enablePhysics(true);
        Object->setCollisionProfile("BlockAll");
        //        Object->SetActorEnableCollision(true);

        Grabbable_Brick = nullptr;
        Object_NearBy = nullptr;
        Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setInteractButtonVisibilty(ESlateVisibility::Hidden);

    }

    Keeping_Bricks = false;
    Object->Grabbed = false;

}



void IBuilderInterface::plugObject(ABrick* GrabbedBrick, IPlugInterface* TargetPlugable, int PluginIndex,
    const FRotator& TargetRotation, const FVector& TargetLocation, const FRotator& OffsetRotation, const FVector& OffsetLocation)
{

    if (TargetPlugable != nullptr && GrabbedBrick != Cast<ABrick>(TargetPlugable))
    {
        TargetPlugable->plugTheItem(GrabbedBrick, PluginIndex, OffsetRotation, OffsetLocation);
    }
    else {
        GrabbedBrick->putTheItem(TargetLocation, TargetRotation);
    }

}



void IBuilderInterface::giveOffsetRotation()
{
    if (OffSet_Location == FVector(0, 0, 0)) {
        if (OffSet_Rotation.Yaw == 360)
            OffSet_Rotation.Yaw = 0;

        OffSet_Rotation.Yaw += 90;
    }

}

void IBuilderInterface::giveOffsetLocation()
{

    int offset = 0;

    if (Ghost) {
        Offset_Step += 1;

        if (Pivot_Width != 0)
            offset = Offset_Step % Pivot_Width;
        else
            offset = 0;


        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, FString::FromInt(Pivot_Width));
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::FromInt(Offset_Step));
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::FromInt(offset));


        if (Ghost->IsVisible()) {
            OffSet_Location -= FVector(0, 25, 0);

        }


        if (offset == 0) {
            OffSet_Location = FVector(0, 0, 0);
            Offset_Step = 0;
        }


    }
}

void IBuilderInterface::lookForBuildingSpace(AActor* Target, FHitResult& OutHit)
{

    IPlugInterface* target_plugable = Cast<IPlugInterface>(Target);

    if (target_plugable)
    {

        if (Target_Plugable_Item != target_plugable) {
            OffSet_Location = FVector(0, 0, 0);
            OffSet_Rotation = FRotator(0, 0, 0);
        }

        Target_Plugable_Item = target_plugable;

        if (Grabbable_Brick)
            Brick_Plugable = target_plugable->highLightPlugin(Ghost, Ghost_Possible_Material, Ghost_Imposible_Material,
                Grabbable_Brick, OutHit, OffSet_Rotation, OffSet_Location, Ghost_Overlapped_Brick);

        if (!Keeping_Bricks)
            Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setInteractButtonVisibilty(ESlateVisibility::Visible);


    }
    else
    {
        int loc_grid_interval = 10;


        if (Grabbable_Brick && Ghost && OutHit.IsValidBlockingHit()) {
            Ghost->SetMaterial(0, Ghost_Possible_Material);
            Ghost->SetMaterial(1, Ghost_Possible_Material);
            if (Grabbable_Brick->getBrickMesh())
                Ghost->SetStaticMesh(Grabbable_Brick->getBrickMesh());

            int x = OutHit.ImpactPoint.X / loc_grid_interval;
            x = x * loc_grid_interval;

            int y = OutHit.ImpactPoint.Y / loc_grid_interval;
            y = y * loc_grid_interval;

            FVector putting_location = FVector(x, y, OutHit.ImpactPoint.Z);

            Ghost->SetVisibility(true);
            Ghost->SetWorldLocationAndRotation(putting_location,
                OutHit.GetActor()->GetActorRotation());

            if (Ghost->GetStaticMesh()) {
                FString name = Ghost->GetStaticMesh()->GetName();
                if (name.Find("Low") >= 0)
                    Ghost->AddLocalOffset(FVector(0, 0, 2));
                else
                    Ghost->AddLocalOffset(FVector(0, 0, 24));
            }


        }


        //if (Ghost_Component != nullptr)
        //{
        //    Ghost_Component->SetVisibility(false);
        //}
        Target_Plugable_Item = nullptr;

        Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setInteractButtonVisibilty(ESlateVisibility::Hidden);

    }

}

void IBuilderInterface::checkGhostItemOverlap(AActor* OtherActor)
{
    ABrick* overlapped_brick = Cast<ABrick>(OtherActor);

    if (overlapped_brick != nullptr)
    {
        Ghost_Overlapped_Brick = overlapped_brick;
    }
}
