// ©, 2021, Erik Jastad. All Rights Reserved


#include "DragTemplates.h"
#include "../../Definitions.h"

void DragTemplate::Cube(TArray<FTransform> * XForm, FVector * Scale)
{
	// Get the drag direction signs
	float signX = (Scale->X >= 0) ? signX = -1 : signX = 1;
	float signY = (Scale->Y >= 0) ? signY = 1 : signY = -1;
	float signZ = (Scale->Z >= 0) ? signZ = 1 : signZ = -1;
	// Calculate the drag scale from the extent scale that is passed in, this is used to scale for each HISM instance, only XY is captured in this.
	FVector DScale = *Scale / XYSIZE; DScale = DScale.GetAbs(); DScale.X = fmax(DScale.X - 1, 1); DScale.Y = fmax(DScale.Y - 1, 1); DScale.Z = abs(Scale->Z); DScale.Z /= ZFLOORHALF;
	// Get the Z start and end locations in world space
	float zCursor = DScale.Z * signZ; zCursor *= ZFLOORHALF; zCursor += (Scale->Z >= 0) ? ZFLOORSIZE : 0;
	float zStart = DScale.Z * signZ; zStart *= -ZFLOORHALF;
	// Get the Z drag scale
	float zScale = DScale.Z; zScale *= ZFLOORSCALE; zScale += (Scale->Z >= 0) ? (ZFLOORSCALE - 1) : -1;

	// add transforms for a cube drag setup, each index is 1 to 1 with HISMList index
	XForm[0].Add(FTransform(FRotator(0.0), FVector(0, 0, zStart), FVector(DScale.X, DScale.Y, 1)));
	XForm[0].Add(FTransform(FRotator(0.0), FVector(0, 0, zCursor), FVector(DScale.X, DScale.Y, 1)));
	XForm[0].Add(FTransform(FRotator(0.0), FVector(0, Scale->Y * signY, (Scale->Z >= 0) ? ZFLOORHALF : 0), FVector(DScale.X, 1, zScale)));
	XForm[0].Add(FTransform(FRotator(0.0), FVector(0, -Scale->Y * signY, (Scale->Z >= 0) ? ZFLOORHALF : 0), FVector(DScale.X, 1, zScale)));
	XForm[0].Add(FTransform(FRotator(0.0), FVector(Scale->X * signX, 0, (Scale->Z >= 0) ? ZFLOORHALF : 0), FVector(1, DScale.Y, zScale)));
	XForm[0].Add(FTransform(FRotator(0.0), FVector(-Scale->X * signX, 0, (Scale->Z >= 0) ? ZFLOORHALF : 0), FVector(1, DScale.Y, zScale)));

	XForm[1].Add(FTransform(FRotator(0.0), FVector(-Scale->X * signX, 0, (Scale->Z >= 0) ? zStart : zCursor), FVector(1, DScale.Y, 1)));
	XForm[1].Add(FTransform(FRotator(0, 180, 0), FVector(Scale->X * signX, 0, (Scale->Z >= 0) ? zStart : zCursor), FVector(1, DScale.Y, 1)));
	XForm[1].Add(FTransform(FRotator(0, 90, 0), FVector(0, Scale->Y * signY, (Scale->Z >= 0) ? zStart : zCursor), FVector(1, DScale.X, 1)));
	XForm[1].Add(FTransform(FRotator(0, 270, 0), FVector(0, -Scale->Y * signY, (Scale->Z >= 0) ? zStart : zCursor), FVector(1, DScale.X, 1)));

	XForm[1].Add(FTransform(FRotator(180, 180, 0), FVector(-Scale->X * signX, 0, (Scale->Z >= 0) ? zCursor : zStart), FVector(1, DScale.Y, 1)));
	XForm[1].Add(FTransform(FRotator(180, 0, 0), FVector(Scale->X * signX, 0, (Scale->Z >= 0) ? zCursor : zStart), FVector(1, DScale.Y, 1)));
	XForm[1].Add(FTransform(FRotator(180, 270, 0), FVector(0, Scale->Y * signY, (Scale->Z >= 0) ? zCursor : zStart), FVector(1, DScale.X, 1)));
	XForm[1].Add(FTransform(FRotator(180, 90, 0), FVector(0, -Scale->Y * signY, (Scale->Z >= 0) ? zCursor : zStart), FVector(1, DScale.X, 1)));

	XForm[1].Add(FTransform(FRotator(0, 180, 90), FVector(Scale->X * signX, Scale->Y * signY, (Scale->Z >= 0) ? ZFLOORHALF : 0), FVector(1, zScale, 1)));
	XForm[1].Add(FTransform(FRotator(0, 90, 90), FVector(-Scale->X * signX, Scale->Y * signY, (Scale->Z >= 0) ? ZFLOORHALF : 0), FVector(1, zScale, 1)));
	XForm[1].Add(FTransform(FRotator(0, 270, 90), FVector(Scale->X * signX, -Scale->Y * signY, (Scale->Z >= 0) ? ZFLOORHALF : 0), FVector(1, zScale, 1)));
	XForm[1].Add(FTransform(FRotator(0, 0, 90), FVector(-Scale->X * signX, -Scale->Y * signY, (Scale->Z >= 0) ? ZFLOORHALF : 0), FVector(1, zScale, 1)));

	XForm[2].Add(FTransform(FRotator(180, 0, 0), FVector(Scale->X * signX, Scale->Y * signY, (Scale->Z >= 0) ? zCursor : zStart), FVector(1)));
	XForm[2].Add(FTransform(FRotator(180, 90, 0), FVector(Scale->X * signX, -Scale->Y * signY, (Scale->Z >= 0) ? zCursor : zStart), FVector(1)));
	XForm[2].Add(FTransform(FRotator(180, 270, 0), FVector(-Scale->X * signX, Scale->Y * signY, (Scale->Z >= 0) ? zCursor : zStart), FVector(1)));
	XForm[2].Add(FTransform(FRotator(180, 180, 0), FVector(-Scale->X * signX, -Scale->Y * signY, (Scale->Z >= 0) ? zCursor : zStart), FVector(1)));

	XForm[2].Add(FTransform(FRotator(0, 90, 0), FVector(Scale->X * signX, Scale->Y * signY, (Scale->Z >= 0) ? zStart : zCursor), FVector(1)));
	XForm[2].Add(FTransform(FRotator(0, 180, 0), FVector(Scale->X * signX, -Scale->Y * signY, (Scale->Z >= 0) ? zStart : zCursor), FVector(1)));
	XForm[2].Add(FTransform(FRotator(0, 0, 0), FVector(-Scale->X * signX, Scale->Y * signY, (Scale->Z >= 0) ? zStart : zCursor), FVector(1)));
	XForm[2].Add(FTransform(FRotator(0, 270, 0), FVector(-Scale->X * signX, -Scale->Y * signY, (Scale->Z >= 0) ? zStart : zCursor), FVector(1)));
}




void DragTemplate::Cylinder(TArray<FTransform> * XForm, FVector * Scale)
{
	//Get Z sign
	float signZ = (Scale->Z >= 0) ? signZ = 1 : signZ = -1;
	// Calculate the drag scale from the extent scale that is passed in, this is used to scale for each HISM instance, only XY is captured in this.
	FVector DScale = *Scale / XYSIZE; DScale = DScale.GetAbs(); DScale.X = fmax(DScale.X - 1, 1); DScale.Y = fmax(DScale.Y - 1, 1); DScale.Z = abs(Scale->Z); DScale.Z /= ZFLOORHALF;
	// Get the Z start and end locations in world space
	float zCursor = DScale.Z * signZ; zCursor *= ZFLOORHALF; zCursor += (Scale->Z >= 0) ? ZFLOORSIZE : 0;
	float zStart = DScale.Z * signZ; zStart *= -ZFLOORHALF;
	// Get the Z drag scale
	float zScale = DScale.Z; zScale *= ZFLOORSCALE; zScale += (Scale->Z >= 0) ? (ZFLOORSCALE - 1) : -1;

	
	float AxisOffset;
	int LoopLimit;
	//Get Scaler based on greater drag direction
	float iScaler = (DScale.Y > DScale.X) ? DScale.X : DScale.Y;
	FVector iScale = (DScale.Y > DScale.X) ? FVector(1, DScale.Y, 1) : FVector(DScale.X, 1, 1);
	FVector isLocation = (DScale.Y > DScale.X) ? FVector(XYSIZE, 0, zStart) : FVector(0, XYSIZE, zStart);
	int VScale;

	// If the drag scale is odd
	if (int(iScaler) % 2)
	{
		// Floor and Ceiling
		XForm[0].Add(FTransform(FRotator(0.0), FVector(0, 0, zStart), iScale));
		XForm[0].Add(FTransform(FRotator(0.0), FVector(0, 0, zCursor), iScale));
		isLocation = (DScale.Y > DScale.X) ? FVector(0, (DScale.Y * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0)) : FVector((DScale.X * XYSIZE) + XYSIZE, 0, ((Scale->Z >= 0) ? ZFLOORHALF : 0));
		// Walls
		XForm[0].Add(FTransform(FRotator(0.0), isLocation, FVector(1, 1, zScale)));
		XForm[0].Add(FTransform(FRotator(0.0), isLocation * FVector(-1, -1, 1), FVector(1, 1, zScale)));
		//Trim Wedges
		isLocation.Z = (Scale->Z >= 0) ? zStart : zCursor;
		XForm[1].Add(FTransform(FRotator(0, ((DScale.Y > DScale.X) ? 90 : 0), 0), isLocation, FVector(1, fmin(5, ((DScale.Y > DScale.X) ? DScale.X : DScale.Y)), 1)));
		XForm[1].Add(FTransform(FRotator(0, ((DScale.Y > DScale.X) ? 270 : 180), 0), isLocation * FVector(-1, -1, 1), FVector(1, fmin(5, ((DScale.Y > DScale.X) ? DScale.X : DScale.Y)), 1)));
		isLocation.Z = (Scale->Z >= 0) ? zCursor : zStart;
		XForm[1].Add(FTransform(FRotator(180, ((DScale.Y > DScale.X) ? 90 : 0), 0), isLocation * FVector(-1, -1, 1), FVector(1, fmin(5, ((DScale.Y > DScale.X) ? DScale.X : DScale.Y)), 1)));
		XForm[1].Add(FTransform(FRotator(180, ((DScale.Y > DScale.X) ? 270 : 180), 0), isLocation, FVector(1, fmin(5, ((DScale.Y > DScale.X) ? DScale.X : DScale.Y)), 1)));

		AxisOffset = WORLDUNITSIZE;
		LoopLimit = iScaler * 0.5;
	}
	else
	{
		// Floor and Ceiling
		XForm[0].Add(FTransform(FRotator(0.0), isLocation * (FVector(-1, -1, 1)), iScale));
		XForm[0].Add(FTransform(FRotator(0.0), isLocation, iScale));
		isLocation.Z = zCursor;
		XForm[0].Add(FTransform(FRotator(0.0), isLocation * (FVector(-1, -1, 1)), iScale));
		XForm[0].Add(FTransform(FRotator(0.0), isLocation, iScale));
		isLocation = (DScale.Y > DScale.X) ? FVector(XYSIZE, (DScale.Y * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0)) : FVector((DScale.X * XYSIZE) + XYSIZE, XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0));
		// Walls in all four directions
		XForm[0].Add(FTransform(FRotator(0.0), isLocation, FVector(1, 1, zScale)));
		XForm[0].Add(FTransform(FRotator(0.0), isLocation * FVector(1, -1, 1), FVector(1, 1, zScale)));
		XForm[0].Add(FTransform(FRotator(0.0), isLocation * FVector(-1, -1, 1), FVector(1, 1, zScale)));
		XForm[0].Add(FTransform(FRotator(0.0), isLocation * FVector(-1, 1, 1), FVector(1, 1, zScale)));
		// Trim Wedges
		isLocation = (DScale.Y > DScale.X) ? FVector(0, (DScale.Y * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0)) : FVector((DScale.X * XYSIZE) + XYSIZE, 0, ((Scale->Z >= 0) ? zStart : zCursor));
		XForm[1].Add(FTransform(FRotator(0, ((DScale.Y > DScale.X) ? 90 : 0), 0), isLocation, FVector(1, fmin(6, ((DScale.Y > DScale.X) ? DScale.X : DScale.Y)), 1)));
		XForm[1].Add(FTransform(FRotator(0, ((DScale.Y > DScale.X) ? 270 : 180), 0), isLocation * FVector(-1, -1, 1), FVector(1, fmin(6, ((DScale.Y > DScale.X) ? DScale.X : DScale.Y)), 1)));
		isLocation.Z = (Scale->Z >= 0) ? zCursor : zStart;
		XForm[1].Add(FTransform(FRotator(180, ((DScale.Y > DScale.X) ? 90 : 0), 0), isLocation * FVector(-1, -1, 1), FVector(1, fmin(6, ((DScale.Y > DScale.X) ? DScale.X : DScale.Y)), 1)));
		XForm[1].Add(FTransform(FRotator(180, ((DScale.Y > DScale.X) ? 270 : 180), 0), isLocation, FVector(1, fmin(6, ((DScale.Y > DScale.X) ? DScale.X : DScale.Y)), 1)));

		AxisOffset = WORLDUNITSIZE + XYSIZE;
		LoopLimit = (iScaler - 1) * 0.5;
	}
	// Do Floor and Ceiling instances
	for (int i = 0; i < LoopLimit; ++i)
	{
		VScale = fmax((i - 1), 0.0);
		VScale = (DScale.Y > DScale.X) ? (DScale.Y - (VScale * 2)) : (DScale.X - (VScale * 2));

		VScale = (VScale < 5) ? 5 : VScale;
		iScale = (DScale.Y > DScale.X) ? FVector(1, VScale, 1) : FVector(VScale, 1, 1);
		isLocation = (DScale.Y > DScale.X) ? FVector(AxisOffset, 0, zStart) : FVector(0, AxisOffset, zStart);

		XForm[0].Add(FTransform(FRotator(0.0), isLocation, iScale));
		XForm[0].Add(FTransform(FRotator(0.0), isLocation * (FVector(-1, -1, 1)), iScale));
		isLocation.Z = zCursor;
		XForm[0].Add(FTransform(FRotator(0.0), isLocation, iScale));
		XForm[0].Add(FTransform(FRotator(0.0), isLocation * (FVector(-1, -1, 1)), iScale));
		// Add Walls
		isLocation = (DScale.Y > DScale.X) ? FVector(AxisOffset, (iScale.Y * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0)) : FVector((iScale.X * XYSIZE) + XYSIZE, AxisOffset, ((Scale->Z >= 0) ? ZFLOORHALF : 0));
		if ((i - 1) <= 0)
		{
			//Add remaining walls needed to meet width of 5, could probably moved into just scaling the central wall instantiated above outside the for loop
			XForm[0].Add(FTransform(FRotator(0), isLocation, FVector(1, 1, zScale)));
			XForm[0].Add(FTransform(FRotator(0), isLocation * FVector(-1, 1, 1), FVector(1, 1, zScale)));
			XForm[0].Add(FTransform(FRotator(0), isLocation * FVector(1, -1, 1), FVector(1, 1, zScale)));
			XForm[0].Add(FTransform(FRotator(0), isLocation * FVector(-1, -1, 1), FVector(1, 1, zScale)));
		}
		else
		{
			// Add Inner Wedge Walls
			XForm[1].Add(FTransform(FRotator(0, 270, 90), isLocation, FVector(1, zScale, 1)));
			XForm[1].Add(FTransform(FRotator(0, 0, 90), isLocation * FVector(-1, 1, 1), FVector(1, zScale, 1)));
			XForm[1].Add(FTransform(FRotator(0, 180, 90), isLocation * FVector(1, -1, 1), FVector(1, zScale, 1)));
			XForm[1].Add(FTransform(FRotator(0, 90, 90), isLocation * FVector(-1, -1, 1), FVector(1, zScale, 1)));

			// Add Inner Corners
			isLocation.Z = (Scale->Z >= 0) ? zStart : zCursor;
			XForm[3].Add(FTransform(FRotator(0, 0, 0), isLocation, FVector(1, 1, 1)));
			XForm[3].Add(FTransform(FRotator(0, 180, 0), isLocation * FVector(-1, -1, 1), FVector(1, 1, 1)));
			XForm[3].Add(FTransform(FRotator(0, 270, 0), isLocation * FVector(1, -1, 1), FVector(1, 1, 1)));
			XForm[3].Add(FTransform(FRotator(0, 90, 0), isLocation * FVector(-1, 1, 1), FVector(1, 1, 1)));
			isLocation.Z = (Scale->Z >= 0) ? zCursor : zStart;
			XForm[3].Add(FTransform(FRotator(180, 270, 0), isLocation, FVector(1, 1, 1)));
			XForm[3].Add(FTransform(FRotator(180, 90, 0), isLocation * FVector(-1, -1, 1), FVector(1, 1, 1)));
			XForm[3].Add(FTransform(FRotator(180, 180, 0), isLocation * FVector(1, -1, 1), FVector(1, 1, 1)));
			XForm[3].Add(FTransform(FRotator(180, 0, 0), isLocation * FVector(-1, 1, 1), FVector(1, 1, 1)));

			// Add Outer Wedge Walls
			isLocation.Z = ((Scale->Z >= 0) ? ZFLOORHALF : 0);
			isLocation += (DScale.Y > DScale.X) ? FVector(0, WORLDUNITSIZE, 0) : FVector(WORLDUNITSIZE, 0, 0);
			XForm[1].Add(FTransform(FRotator(0, 90, 90), isLocation, FVector(1, zScale, 1)));
			XForm[1].Add(FTransform(FRotator(0, 180, 90), isLocation * FVector(-1, 1, 1), FVector(1, zScale, 1)));
			XForm[1].Add(FTransform(FRotator(0, 0, 90), isLocation * FVector(1, -1, 1), FVector(1, zScale, 1)));
			XForm[1].Add(FTransform(FRotator(0, 270, 90), isLocation * FVector(-1, -1, 1), FVector(1, zScale, 1)));
			// Add Outer Corners
			isLocation.Z = (Scale->Z >= 0) ? zStart : zCursor;
			XForm[2].Add(FTransform(FRotator(0, 0, 0), isLocation, FVector(1, 1, 1)));
			XForm[2].Add(FTransform(FRotator(0, 180, 0), isLocation * FVector(-1, -1, 1), FVector(1, 1, 1)));
			XForm[2].Add(FTransform(FRotator(0, 270, 0), isLocation * FVector(1, -1, 1), FVector(1, 1, 1)));
			XForm[2].Add(FTransform(FRotator(0, 90, 0), isLocation * FVector(-1, 1, 1), FVector(1, 1, 1)));
			isLocation.Z = (Scale->Z >= 0) ? zCursor : zStart;
			XForm[2].Add(FTransform(FRotator(180, 270, 0), isLocation, FVector(1, 1, 1)));
			XForm[2].Add(FTransform(FRotator(180, 90, 0), isLocation * FVector(-1, -1, 1), FVector(1, 1, 1)));
			XForm[2].Add(FTransform(FRotator(180, 180, 0), isLocation * FVector(1, -1, 1), FVector(1, 1, 1)));
			XForm[2].Add(FTransform(FRotator(180, 0, 0), isLocation * FVector(-1, 1, 1), FVector(1, 1, 1)));
		}
		AxisOffset += WORLDUNITSIZE;
	}
	// Add Ending Wedges
	isLocation = (DScale.Y > DScale.X) ? FVector(AxisOffset, (iScale.Y * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0)) : FVector((iScale.X * XYSIZE) + XYSIZE, AxisOffset, ((Scale->Z >= 0) ? ZFLOORHALF : 0));
	XForm[1].Add(FTransform(FRotator(0, 90, 90), isLocation, FVector(1, zScale, 1)));
	XForm[1].Add(FTransform(FRotator(0, 180, 90), isLocation * FVector(-1, 1, 1), FVector(1, zScale, 1)));
	XForm[1].Add(FTransform(FRotator(0, 0, 90), isLocation * FVector(1, -1, 1), FVector(1, zScale, 1)));
	XForm[1].Add(FTransform(FRotator(0, 270, 90), isLocation * FVector(-1, -1, 1), FVector(1, zScale, 1)));

	// Add Ending Corners
	isLocation.Z = (Scale->Z >= 0) ? zStart : zCursor;
	XForm[2].Add(FTransform(FRotator(0, 0, 0), isLocation, FVector(1, 1, 1)));
	XForm[2].Add(FTransform(FRotator(0, 180, 0), isLocation * FVector(-1, -1, 1), FVector(1, 1, 1)));
	XForm[2].Add(FTransform(FRotator(0, 270, 0), isLocation * FVector(1, -1, 1), FVector(1, 1, 1)));
	XForm[2].Add(FTransform(FRotator(0, 90, 0), isLocation * FVector(-1, 1, 1), FVector(1, 1, 1)));
	isLocation.Z = (Scale->Z >= 0) ? zCursor : zStart;
	XForm[2].Add(FTransform(FRotator(180, 270, 0), isLocation, FVector(1, 1, 1)));
	XForm[2].Add(FTransform(FRotator(180, 90, 0), isLocation * FVector(-1, -1, 1), FVector(1, 1, 1)));
	XForm[2].Add(FTransform(FRotator(180, 180, 0), isLocation * FVector(1, -1, 1), FVector(1, 1, 1)));
	XForm[2].Add(FTransform(FRotator(180, 0, 0), isLocation * FVector(-1, 1, 1), FVector(1, 1, 1)));

	// Add Ending Walls
	isLocation = (DScale.Y > DScale.X) ? FVector(AxisOffset, 0, ((Scale->Z >= 0) ? ZFLOORHALF : 0)) : FVector(0, AxisOffset, ((Scale->Z >= 0) ? ZFLOORHALF : 0));
	iScale = (DScale.Y > DScale.X) ? FVector(1, iScale.Y, zScale) : FVector(iScale.X, 1, zScale);
	XForm[0].Add(FTransform(FRotator(0), isLocation, iScale));
	XForm[0].Add(FTransform(FRotator(0), isLocation * FVector(-1, -1, 1), iScale));

	// Add Wedges above and below Ending Walls
	isLocation.Z = (Scale->Z >= 0) ? zStart : zCursor;
	XForm[1].Add(FTransform(FRotator(0, ((DScale.Y > DScale.X) ? 0 : 90), 0), isLocation, FVector(1, ((DScale.Y > DScale.X) ? iScale.Y : iScale.X), 1)));
	XForm[1].Add(FTransform(FRotator(0, ((DScale.Y > DScale.X) ? 180 : 270), 0), isLocation * FVector(-1, -1, 1), FVector(1, ((DScale.Y > DScale.X) ? iScale.Y : iScale.X), 1)));
	isLocation.Z = (Scale->Z >= 0) ? zCursor : zStart;
	XForm[1].Add(FTransform(FRotator(180, ((DScale.Y > DScale.X) ? 0 : 90), 0), isLocation * FVector(-1, -1, 1), FVector(1, ((DScale.Y > DScale.X) ? iScale.Y : iScale.X), 1)));
	XForm[1].Add(FTransform(FRotator(180, ((DScale.Y > DScale.X) ? 180 : 270), 0), isLocation, FVector(1, ((DScale.Y > DScale.X) ? iScale.Y : iScale.X), 1)));
}




void DragTemplate::Sphere(TArray<FTransform> * XForm, FVector * Scale)
{
	//Get Z sign
	float signZ = (Scale->Z >= 0) ? signZ = 1 : signZ = -1;
	// Calculate the drag scale from the extent scale that is passed in, this is used to scale for each HISM instance, only XY is captured in this.
	FVector DScale = *Scale / XYSIZE; DScale = DScale.GetAbs(); DScale.X = fmax(DScale.X - 1, 1); DScale.Y = fmax(DScale.Y - 1, 1); DScale.Z = abs(Scale->Z); DScale.Z /= ZFLOORHALF;
	// Get the Z start and end locations in world space
	float zCursor = DScale.Z * signZ; zCursor *= ZFLOORHALF; zCursor += (Scale->Z >= 0) ? ZFLOORSIZE : 0;
	float zStart = DScale.Z * signZ; zStart *= -ZFLOORHALF;
	// Get the Z drag scale
	float zScale = DScale.Z; zScale *= ZFLOORSCALE; zScale += (Scale->Z >= 0) ? (ZFLOORSCALE - 1) : -1;


//	float AxisOffset;
//	int LoopLimit;
	//Get Scaler based on greater drag direction
	float iScaler = (DScale.Y > DScale.X) ? DScale.X : DScale.Y;
	FVector iScale = (DScale.Y > DScale.X) ? FVector(DScale.X, DScale.Y, 1) : FVector(DScale.X, DScale.Y, 1);
	FVector isLocation = (DScale.Y > DScale.X) ? FVector(XYSIZE, 0, zStart) : FVector(0, XYSIZE, zStart);
//	int VScale;

	int sLoop = 2 + (ceil(fmax(((DScale.Z - 1)/2), 0)) * 8);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("X and Y = %d"), sLoop)); 

	//Floor
	XForm[0].Add(FTransform(FRotator(0.0), FVector(0, 0, zStart), iScale));
	XForm[0].Add(FTransform(FRotator(0.0), FVector(0, 0, zCursor), iScale));
}
