// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShipBuilder/ShipManager.h"
#include "Characters/GDBaseCharacter.h"
#include "Enums.h"
#include "ShipBuilder/UI/ShipBuildWidget.h"
#include "GDPlayerController.generated.h"


/**
 * Player Controller for Gravity Drive
 */
UCLASS()
class GRAVITYDRIVE_API AGDPlayerController : public APlayerController
{
	GENERATED_BODY()
	public:
		AGDPlayerController();
		virtual void BeginPlay() override;
		void UpdateControlRotationYaw(float Value);
		void UpdateControlRotationPitch(float Value);
		void UpdateCanRotate();
		void HandleInteract();
		
		// Hud subclass reference, set this in the BP child.
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UShipBuildWidget> BuildHudClass;
		// Reference to spawned subclass
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UShipBuildWidget* BuildHud;

	private:
		void SetPlayerChar();
		//Timer for get pawn
		FTimerHandle PlayerReadyTimer;
		//Enum current pawn mode
		PawnMode ePAWNMODE;
		//Bool mmb state
		bool bMMB;
		//Ship manager Reference
		AShipManager* cSHIPMANAGER;
		//Player character reference
		AGDBaseCharacter* cPLAYERCHAR;

};
