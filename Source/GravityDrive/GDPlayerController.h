// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShipFlight/ShipFlight.h"
#include "Characters/GDBaseCharacter.h"
#include "Definitions.h"
#include "ShipBuilder/UI/ShipBuildWidget.h"
#include "GDPlayerController.generated.h"


/**
 * Player Controller for Gravity Drive
 * Currently this controller primarily just controls which pawn is possessed, showing mouse cursor toggle, and the hud displayed
 */
UCLASS()
class GRAVITYDRIVE_API AGDPlayerController : public APlayerController
{
	GENERATED_BODY()
	public:
		AGDPlayerController(); // Default Constructor
		virtual void BeginPlay() override; //BeginPlay called when the game starts
		void CyclePossession(); // Handles swapping pawn control
		
		// Hud subclass reference, set this in the BP child.
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UShipBuildWidget> BuildHudClass;
		// Reference to spawned subclass
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UShipBuildWidget* BuildHud;

	private:
		//Enum current pawn mode
		PawnMode ePAWNMODE;

		//Ship manager Reference
		AShipManager* cSHIPMANAGER;
		//Player character reference
		AGDBaseCharacter* cPLAYERCHAR;
		//Ship flight reference
		AShipFlight * cSHIPFLIGHT;

};
