#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FGBuildableHologram.h"
#include "FGBuildable.h"
#include <FGConveyorAttachmentHologram.h>
#include "MyConveyorToolsSubsystem.generated.h"

class AFGBlueprintHologram;
class AFGBuildableConveyorBase;

/**
 * Subsystem zur Verwaltung von Blueprint-Hologrammen
 * und automatischem Spiegeln von Förderbändern, Splittern und Mergern.
 */
UCLASS()
class FLIPBLUEPRINT_API UMyConveyorToolsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    /** Flippt alle Buildables in einem Hologramm (Blueprint) */
    UFUNCTION(BlueprintCallable, Category = "Conveyor Tools")
    void FlipHologram(AFGBlueprintHologram* Hologram);
    void ConstructHologram(TArray<AActor*> Tactor);
    bool IsConnectedToMachine(AActor* Actor);

private:
    static bool IsFliped;
};


