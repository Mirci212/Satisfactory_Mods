#include "MyConveyorToolsSubsystem.h"
#include "FGBuildableConveyorBase.h"
#include "FGBuildableAttachmentSplitter.h"
#include "FGBuildableAttachmentMerger.h"
#include "FGBlueprintHologram.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "FGBuildableManufacturer.h"
#include <FGBuildableConveyorBelt.h>

bool UMyConveyorToolsSubsystem::IsFliped = false; // Initialwert

void UMyConveyorToolsSubsystem::FlipHologram(AFGBlueprintHologram* Hologram)
{
    if (!IsValid(Hologram)) return;

    for (auto& Element : Hologram->mBuildableToNewRoot)
    {
        AFGBuildable* Key = Element.Key;
        USceneComponent* Value = Element.Value;

        FString KeyName = Key ? Key->GetName() : TEXT("Null Key");
        FString ValueName = Value ? Value->GetName() : TEXT("Null Value");

        UE_LOG(LogTemp, Warning, TEXT("Key: %s | Value: %s"), *KeyName, *ValueName);
        if (Key && Value && (KeyName.Contains("Merger") || KeyName.Contains("Splitter")))
        {
            // 1. Komponente drehen (relativ zum Parent)
            FRotator CompRotation = Value->GetRelativeRotation();
            CompRotation.Yaw += 180.f;
            Value->SetRelativeRotation(CompRotation);
			Key->ReregisterAllComponents(); // Alle Komponenten erneut registrieren
			//Element.Key->SetActorRotation(CompRotation, ETeleportType::ResetPhysics);
			//Element.Key->OnConstruction(Element.Key->GetActorTransform());
        }

    }
	IsFliped = !IsFliped;
}

void UMyConveyorToolsSubsystem::ConstructHologram(TArray<AActor*> Tactor)
{
    if (!IsFliped) return;
    for (AActor* Actor : Tactor)
    {
        if (!IsValid(Actor))
        {
            UE_LOG(LogTemp, Warning, TEXT("[MyConveyorTools] Skipping - Flip not needed or actor invalid"));
            continue;
        }

        UE_LOG(LogTemp, Warning, TEXT("[MyConveyorTools] Processing actor: %s (%s)"),
            *Actor->GetName(), *Actor->GetClass()->GetName());

        // Typprüfung
        bool bIsSplitter = Actor->GetName().Contains("Splitter");
        bool bIsMerger = Actor->GetName().Contains("Merger");
        bool bIsConveyor = Actor->IsA<AFGBuildableConveyorBase>();

        bool bShouldFlip = bIsSplitter || bIsMerger || (bIsConveyor && !IsConnectedToMachine(Actor));

        UE_LOG(LogTemp, Warning, TEXT("[MyConveyorTools] Type check - Splitter: %d, Merger: %d, Conveyor: %d, Should flip: %d"),
            bIsSplitter, bIsMerger, bIsConveyor, bShouldFlip);

        if (bShouldFlip)
        {
            // Actor Rotation
            FRotator CurrentRotation = Actor->GetActorRotation();
            FRotator NewRotation = CurrentRotation + FRotator(0.f, 180.f, 0.f);
            Actor->SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);

            UE_LOG(LogTemp, Warning, TEXT("[MyConveyorTools] Rotated actor %s. Old: %s, New: %s"),
                *Actor->GetName(), *CurrentRotation.ToString(), *NewRotation.ToString());
            if (auto buildable = Cast<AFGBuildable>(Actor))
            {
                // 1. Explizite Rotation setzen
                buildable->SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);

                // 2. Komponenten-Hierarchie aktualisieren
                if (USceneComponent* RootComp = buildable->GetRootComponent())
                {
                    RootComp->UpdateComponentToWorld();
                    RootComp->MarkRenderStateDirty();

                    // 3. Alle StaticMeshComponents aktualisieren
                    TArray<UStaticMeshComponent*> MeshComps;
                    buildable->GetComponents(MeshComps);

                    for (UStaticMeshComponent* Mesh : MeshComps)
                    {
                        Mesh->RecreateRenderState_Concurrent();
                        Mesh->MarkRenderDynamicDataDirty();
                    }
                }
            }
        }
    }  
    // Flip abgeschlossen
    IsFliped = false;
    UE_LOG(LogTemp, Warning, TEXT("[MyConveyorTools] Flip operation completed for all actors"));
}




bool UMyConveyorToolsSubsystem::IsConnectedToMachine(AActor* Actor)
{
    UE_LOG(LogTemp, Verbose, TEXT("[MyConveyorTools] Checking machine connections for: %s"), *Actor->GetName());

    if (AFGBuildableConveyorBase* Conveyor = Cast<AFGBuildableConveyorBase>(Actor))
    {
        UFGFactoryConnectionComponent* Conn0 = Conveyor->GetConnection0();
        UFGFactoryConnectionComponent* Conn1 = Conveyor->GetConnection1();

        UE_LOG(LogTemp, Verbose, TEXT("[MyConveyorTools] Connection0 valid: %d, Connection1 valid: %d"),
            Conn0 != nullptr, Conn1 != nullptr);

        auto CheckConnection = [](UFGFactoryConnectionComponent* Connection, int Index) -> bool
            {
                UE_LOG(LogTemp, Verbose, TEXT("[MyConveyorTools] Checking connection %d"), Index);

                if (!Connection)
                {
                    UE_LOG(LogTemp, Verbose, TEXT("[MyConveyorTools] Connection %d is null"), Index);
                    return false;
                }

                if (!Connection->IsConnected())
                {
                    UE_LOG(LogTemp, Verbose, TEXT("[MyConveyorTools] Connection %d is not connected"), Index);
                    return false;
                }

                UFGFactoryConnectionComponent* ConnectedComponent = Connection->GetConnection();
                if (!ConnectedComponent)
                {
                    UE_LOG(LogTemp, Verbose, TEXT("[MyConveyorTools] Connected component %d is null"), Index);
                    return false;
                }

                AFGBuildable* ConnectedBuildable = ConnectedComponent->GetOuterBuildable();
                if (!ConnectedBuildable)
                {
                    UE_LOG(LogTemp, Verbose, TEXT("[MyConveyorTools] Connected buildable %d is null"), Index);
                    return false;
                }

                bool bIsManufacturer = ConnectedBuildable->IsA<AFGBuildableManufacturer>();
                UE_LOG(LogTemp, Verbose, TEXT("[MyConveyorTools] Connection %d buildable: %s (IsManufacturer: %d)"),
                    Index, *ConnectedBuildable->GetName(), bIsManufacturer);

                return bIsManufacturer;
            };

        bool bConn0Result = CheckConnection(Conn0, 0);
        bool bConn1Result = CheckConnection(Conn1, 1);

        UE_LOG(LogTemp, Warning, TEXT("[MyConveyorTools] Connection results - 0: %d, 1: %d"),
            bConn0Result, bConn1Result);

        return bConn0Result || bConn1Result;
    }

    UE_LOG(LogTemp, Verbose, TEXT("[MyConveyorTools] Actor is not a conveyor"));
    return false;
}