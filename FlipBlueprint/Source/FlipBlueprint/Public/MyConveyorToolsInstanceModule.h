#pragma once

#include "CoreMinimal.h"
#include "Module/GameInstanceModule.h"
#include "MyConveyorToolsSubsystem.h"
#include "FGBlueprintHologram.h"
#include "Hologram/FGBuildableHologram.h"
#include "MyConveyorToolsInstanceModule.generated.h"

UCLASS()
class FLIPBLUEPRINT_API UMyConveyorToolsInstanceModule : public UGameInstanceModule
{
    GENERATED_BODY()

public:
    virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;

private:
    void SetupHologramHooks();
    void PreInitializeSubsystem();
	void LoadRequiredAssets();
    void RotateAndLockBuildable(AFGBuildable* buildable, const FRotator& newRotation);
};