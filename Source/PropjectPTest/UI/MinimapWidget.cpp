#include "MinimapWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UMinimapWidget::InitializeMinimap(FVector Origin, FVector Extent, FVector2D Size)
{
    VolumeOrigin = Origin;
    VolumeExtent = Extent;
    MinimapSize = Size;

    UE_LOG(LogTemp, Log, TEXT("InitializeMinimap called"));
    UE_LOG(LogTemp, Log, TEXT("Volume Origin: X=%f, Y=%f, Z=%f"), VolumeOrigin.X, VolumeOrigin.Y, VolumeOrigin.Z);
    UE_LOG(LogTemp, Log, TEXT("Volume Extent: X=%f, Y=%f, Z=%f"), VolumeExtent.X, VolumeExtent.Y, VolumeExtent.Z);
    UE_LOG(LogTemp, Log, TEXT("Minimap Size: X=%f, Y=%f"), MinimapSize.X, MinimapSize.Y);
}

void UMinimapWidget::UpdatePlayerIconPosition(FVector PlayerLocation)
{
    UE_LOG(LogTemp, Log, TEXT("UpdatePlayerIconPosition called with X=%f, Y=%f, Z=%f"), PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z);

    if (PlayerIcon)
    {
        if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(PlayerIcon->Slot))
        {
            FVector2D MinimapPosition = ConvertWorldLocationToMinimap(PlayerLocation);

            FVector2D IconSize = CanvasSlot->GetSize();
            MinimapPosition.X -= IconSize.X / 2;
            MinimapPosition.Y -= IconSize.Y / 2;

            FVector2D CanvasSlotPosition = CanvasSlot->GetPosition();
            UE_LOG(LogTemp, Log, TEXT("CanvasSlot Position before update: X=%f, Y=%f"), CanvasSlotPosition.X, CanvasSlotPosition.Y);

            CanvasSlot->SetPosition(MinimapPosition);

            CanvasSlotPosition = CanvasSlot->GetPosition();
            UE_LOG(LogTemp, Log, TEXT("CanvasSlot Position after update: X=%f, Y=%f"), CanvasSlotPosition.X, CanvasSlotPosition.Y);

            UE_LOG(LogTemp, Log, TEXT("PlayerIcon position updated to: X=%f, Y=%f"), MinimapPosition.X, MinimapPosition.Y);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerIcon is not a CanvasPanelSlot"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerIcon is null"));
    }
}

FVector2D UMinimapWidget::ConvertWorldLocationToMinimap(FVector WorldLocation)
{
    if (VolumeExtent.X == 0 || VolumeExtent.Y == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("VolumeExtent has zero dimension"));
        return FVector2D(0, 0);
    }

    FVector RelativeLocation = WorldLocation - VolumeOrigin;

    FVector2D NormalizedLocation;
    NormalizedLocation.X = (RelativeLocation.X + VolumeExtent.X) / (2 * VolumeExtent.X);
    NormalizedLocation.Y = (RelativeLocation.Y + VolumeExtent.Y) / (2 * VolumeExtent.Y);

    // 반전된 Y 축을 고려하지 않은 경우
    // NormalizedLocation.Y = 1.0f - NormalizedLocation.Y;

    FVector2D MinimapCoordinates;
    MinimapCoordinates.X = NormalizedLocation.X * MinimapSize.X;
    MinimapCoordinates.Y = NormalizedLocation.Y * MinimapSize.Y;

    UE_LOG(LogTemp, Log, TEXT("VolumeOrigin: X=%f, Y=%f, Z=%f"), VolumeOrigin.X, VolumeOrigin.Y, VolumeOrigin.Z);
    UE_LOG(LogTemp, Log, TEXT("VolumeExtent: X=%f, Y=%f, Z=%f"), VolumeExtent.X, VolumeExtent.Y, VolumeExtent.Z);
    UE_LOG(LogTemp, Log, TEXT("RelativeLocation: X=%f, Y=%f, Z=%f"), RelativeLocation.X, RelativeLocation.Y, RelativeLocation.Z);
    UE_LOG(LogTemp, Log, TEXT("NormalizedLocation: X=%f, Y=%f"), NormalizedLocation.X, NormalizedLocation.Y);
    UE_LOG(LogTemp, Log, TEXT("MinimapCoordinates: X=%f, Y=%f"), MinimapCoordinates.X, MinimapCoordinates.Y);

    return MinimapCoordinates;
}
