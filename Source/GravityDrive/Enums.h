// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"

/**
 * This enum is used to determine which mode
 */

//This Enum is used to indicate which pawn mode the player is currently in
enum PawnMode {
	default,
	character,
	shipbuilder,
	starship,
	turret,
	pilot
};
