#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapWidget.generated.h"

UCLASS()
class PROPJECTPTEST_API UMinimapWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitializeMinimap(FVector Origin, FVector Extent, FVector2D Size);
    void UpdatePlayerIconPosition(FVector PlayerLocation);

protected:
    FVector VolumeOrigin;
    FVector VolumeExtent;
    FVector2D MinimapSize;

    FVector2D ConvertWorldLocationToMinimap(FVector WorldLocation);

    UPROPERTY(meta = (BindWidget))
    class UImage* PlayerIcon;
};
