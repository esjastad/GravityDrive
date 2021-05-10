// ©, 2021, Erik Jastad. All Rights Reserved


#include "GDPlayerController.h"
// Default constructor
AGDPlayerController::AGDPlayerController()
{
	ePAWNMODE = character; //default mode
}

// Called when game play starts
void AGDPlayerController::BeginPlay() 
{
	Super::BeginPlay();
	// Bind user inputs to relative functions
	InputComponent->BindAction("Interact", IE_Pressed, this, &AGDPlayerController::CyclePossession);
	
	// Get the Ship Manager Ref
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShipManager::StaticClass(), FoundActors);
	cSHIPMANAGER = Cast<AShipManager>(FoundActors[0]);
	// Get the Ship Flyer Ref
	FoundActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShipFlight::StaticClass(), FoundActors);
	cSHIPFLIGHT = Cast<AShipFlight>(FoundActors[0]);

	// Create the ship build hud
	BuildHud = CreateWidget<UShipBuildWidget>(this, BuildHudClass);
	BuildHud->SetShipReference(cSHIPMANAGER);

	//Get the Player Ref. Since the player is spawned after the controller; a delay is needed to obtain this reference
	_sleep(100);
	cPLAYERCHAR = Cast<AGDBaseCharacter>(GetPawn());
}

// Currently cycles which pawn is possessed, F key, Input is called "Interact"
// Toggles pawn controlled, showing mouse cursor and hud displaying
void AGDPlayerController::CyclePossession()
{
	switch (ePAWNMODE)
	{
		case character: 
			ePAWNMODE = starship;
			Possess(cSHIPMANAGER);
			BuildHud->AddToViewport();
			bShowMouseCursor = true;	
			break;
		case shipbuilder: 
			ePAWNMODE = character;
			Possess(cPLAYERCHAR);
			bShowMouseCursor = false;
			break;
		case starship: 
			ePAWNMODE = shipbuilder;
			Possess(cSHIPFLIGHT);
			BuildHud->RemoveFromViewport();
			bShowMouseCursor = false;	
			break;
		case turret:
			break;
		case pilot:
			break;
	}
}