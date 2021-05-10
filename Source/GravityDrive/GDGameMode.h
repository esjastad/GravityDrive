// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GDPlayerController.h"
#include "Characters/GDBaseCharacter.h"
#include "GDGameMode.generated.h"

/**
 * GravityDrive Gamemode set default playercontroller
 */
UCLASS()
class GRAVITYDRIVE_API AGDGameMode : public AGameModeBase
{
	GENERATED_BODY()
	public:
		virtual void StartPlay() override;
		AGDGameMode();

};
