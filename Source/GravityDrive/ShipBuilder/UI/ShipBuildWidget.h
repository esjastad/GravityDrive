// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../ShipManager.h"
#include "ShipBuildWidget.generated.h"

/**
 * UI ShipBuilding Widget for the ship manager
 * Contains all UI info and controls for Ship Manager/Builder
 */

UCLASS()
class GRAVITYDRIVE_API UShipBuildWidget : public UUserWidget
{
	GENERATED_BODY()
	public:
		void OnInitialized();
		void SetShipReference(AShipManager * NewRef);

		UFUNCTION(BlueprintCallable)
		void UpdateBuildState();
		UFUNCTION(BlueprintCallable)
		void SetSegType(int type);
		UFUNCTION(BlueprintImplementableEvent)
		void DrawDragDimensions(FVector Scale, FVector2D Center, FVector2D Xoffset, FVector2D Yoffset, FVector2D Zoffset);
		UFUNCTION(BlueprintImplementableEvent)
		void RemoveDragDimensions();

		UFUNCTION(BlueprintCallable)
		void SetBuildMode(int mode);

		AShipManager * ShipManagerRef;
		SegmentType SegType;
		
};
