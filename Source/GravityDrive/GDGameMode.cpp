// ©, 2021, Erik Jastad. All Rights Reserved


#include "GDGameMode.h"

// Default Constructor
AGDGameMode::AGDGameMode()
{
	PlayerControllerClass = AGDPlayerController::StaticClass();
}

// Called when the game starts
void AGDGameMode::StartPlay()
{
	Super::StartPlay();
}

