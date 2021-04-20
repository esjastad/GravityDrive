// ©, 2021, Erik Jastad. All Rights Reserved


#include "GDPlayerController.h"
// Default constructor
AGDPlayerController::AGDPlayerController()
{
	bMMB = false;
	ePAWNMODE = character;
}

// Called when game play starts
void AGDPlayerController::BeginPlay() 
{
	Super::BeginPlay();
	// Bind user inputs to relative functions
	InputComponent->BindAction("MiddleMouse", IE_Pressed, this, &AGDPlayerController::UpdateCanRotate);
	InputComponent->BindAction("MiddleMouse", IE_Released, this, &AGDPlayerController::UpdateCanRotate);
	InputComponent->BindAction("Interact", IE_Pressed, this, &AGDPlayerController::HandleInteract);

	InputComponent->BindAxis("LookRight", this, &AGDPlayerController::UpdateControlRotationYaw);
	InputComponent->BindAxis("LookUp", this, &AGDPlayerController::UpdateControlRotationPitch);
	
	// Get the ship manager
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShipManager::StaticClass(), FoundActors);
	cSHIPMANAGER = Cast<AShipManager>(FoundActors[0]);

	// Set a small time delay to grab a reference to the player pawn that is spawned in
	GetWorldTimerManager().SetTimer(PlayerReadyTimer, this, &AGDPlayerController::SetPlayerChar, 0.01f, false, true);

	// Create the ship build hud
	BuildHud = CreateWidget<UShipBuildWidget>(this, BuildHudClass);
	BuildHud->SetShipReference(cSHIPMANAGER);

	ControlRotation.Pitch = 320.0;

}

// Get the current pawn and set a reference
void AGDPlayerController::SetPlayerChar()
{
	cPLAYERCHAR = Cast<AGDBaseCharacter>(GetPawn());
}

// When player presses/releases middle mouse button, invert state variable
void AGDPlayerController::UpdateCanRotate()
{
	bMMB = !bMMB;
}

// Control Player Yaw Rotation
void AGDPlayerController::UpdateControlRotationYaw(float Value)
{
	if (bMMB && Value != 0.0f || !bShowMouseCursor && Value != 0.0f)
	{
		ControlRotation.Yaw += Value;
	}
}

// Control Player Pitch Rotation
void AGDPlayerController::UpdateControlRotationPitch(float Value)
{
	if (bMMB && Value != 0.0f || !bShowMouseCursor && Value != 0.0f)
	{
		ControlRotation.Pitch += Value;
	}
}

// Handle player interaction (currently bound to E key)
void AGDPlayerController::HandleInteract()
{
	switch (ePAWNMODE)
	{
		case character:
			ePAWNMODE = shipbuilder;
			Possess(cSHIPMANAGER);
			BuildHud->AddToViewport();
			bShowMouseCursor = true;
			ControlRotation.Yaw = 0.0;
			ControlRotation.Pitch = 320.0;
			break;
		case shipbuilder:
			ePAWNMODE = character;
			Possess(cPLAYERCHAR);
			BuildHud->RemoveFromViewport();
			bShowMouseCursor = false;
			ControlRotation.Pitch = 320.0;
			break;
		case starship:
			break;
		case turret:
			break;
		case pilot:
			break;
	}
}