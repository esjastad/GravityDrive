// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

namespace DragTemplate
{
	void Cube(TArray<FTransform> * XForm, FVector * Scale, int bias);
	void Cylinder(TArray<FTransform> * XForm, FVector * Scale, int bias);
	void Sphere(TArray<FTransform> * XForm, FVector * Scale, int bias);
}
