// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"

/**
 * Drag Templates, contains the code to fill a transform array for the respective shape function called
 */

namespace DragTemplate
{
	void Cube(TArray<FTransform> * XForm, FVector * Scale, int bias);	// Cube Drag
	void Cylinder(TArray<FTransform> * XForm, FVector * Scale, int bias);	// Cylinder Drag
	void Sphere(TArray<FTransform> * XForm, FVector * Scale, int bias);		// Sphere Drag
}
