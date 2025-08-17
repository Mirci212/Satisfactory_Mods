#include "MyConveyorToolsInstanceModule.h"
#include "Patching/NativeHookManager.h"
#include "MyConveyorToolsSubsystem.h"
#include "FGBlueprintHologram.h"
#include "FGBuildableHologram.h"
#include "FGBuildableConveyorBase.h"
#include "FGBuildableAttachmentSplitter.h"
#include "FGBuildableAttachmentMerger.h"
#include "FGConveyorAttachmentHologram.h"
#include "FGBuildableConveyorBelt.h"
#include "Hologram/FGHologram.h"
#include "EngineUtils.h" // für TActorIterator
#include "Engine/World.h"

void UMyConveyorToolsInstanceModule::DispatchLifecycleEvent(ELifecyclePhase Phase)
{
    Super::DispatchLifecycleEvent(Phase);

    if (Phase == ELifecyclePhase::INITIALIZATION)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MyConveyorTools] Initializing & setting up hooks..."));
        SetupHologramHooks();
    }
}

void UMyConveyorToolsInstanceModule::SetupHologramHooks()
{

    SUBSCRIBE_METHOD_VIRTUAL_AFTER(
        AFGBuildable::PreSerializedToBlueprint,
        GetMutableDefault<AFGBuildable>(),
        [](AFGBuildable* self)
        {
            if (!IsValid(self)) return;
            if (self->IsA<AFGHologram>()) return;

            const FString buildableName = self->GetClass()->GetName();
            const bool bShouldRotate =
                buildableName.Contains(TEXT("Splitter")) ||
                buildableName.Contains(TEXT("Merger")) ||
                self->IsA<AFGBuildableConveyorAttachment>();

            if (!bShouldRotate) return;

            // --- 1. RootComponent rotation ---
            if (USceneComponent* root = self->GetRootComponent())
            {
                root->AddLocalRotation(FRotator(0, 180.f, 0));
                root->MarkRenderStateDirty();
                root->MarkRenderTransformDirty();
                UE_LOG(LogTemp, Warning, TEXT("[Mod] Root component rotated"));
            }

            // --- 2. Update child components ---
            /*TInlineComponentArray<USceneComponent*> sceneComponents;
            self->GetComponents(sceneComponents);

            for (USceneComponent* comp : sceneComponents)
            {
                if (!IsValid(comp)) continue;

                comp->AddLocalRotation(FRotator(0, 180.f, 0));

                if (UFGFactoryConnectionComponent* conn = Cast<UFGFactoryConnectionComponent>(comp))
                {
                    conn->SetRelativeTransform(conn->GetRelativeTransform());
                }

                comp->MarkRenderStateDirty();
                comp->MarkRenderTransformDirty();
                comp->UpdateComponentToWorld();
                comp->RecreatePhysicsState();
                comp->UpdateBounds();
            }*/

            // --- 3. Finale Aktualisierung ---
            self->UserConstructionScript();
            self->ReregisterAllComponents();
            self->ForceNetUpdate();

            UE_LOG(LogTemp, Warning, TEXT("[Mod] Successfully processed buildable: %s"), *self->GetName());
        }
    );








    UE_LOG(LogTemp, Warning, TEXT("[MyConveyorTools] Hologram hook registered successfully."));

}
