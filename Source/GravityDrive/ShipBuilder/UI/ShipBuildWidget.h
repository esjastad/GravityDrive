// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShipBuildWidget.generated.h"

/**
 * UI ShipBuilding Widget for the ship manager
 */
UCLASS()
class GRAVITYDRIVE_API UShipBuildWidget : public UUserWidget
{
	GENERATED_BODY()
	public:
		void OnInitialized();
};
