// ©, 2021, Erik Jastad. All Rights Reserved


#include "DragTemplates.h"
#include "../../Definitions.h"

void DragTemplate::Cube(TArray<FTransform> * XForm, FVector * Scale, int bias)
{
	DragTemplate::Sphere(XForm, Scale, bias);
}



void DragTemplate::Cylinder(TArray<FTransform> * XForm, FVector * Scale, int bias)
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
		XForm[0].Add(FTransform(FRotator(0.0), FVector(0, 0, (Scale->Z >= 0) ? zStart : zCursor), iScale));
		XForm[0].Add(FTransform(FRotator(180,0,0), FVector(0, 0, (Scale->Z >= 0) ? zCursor : zStart), iScale));
		isLocation = (DScale.Y > DScale.X) ? FVector(0, (DScale.Y * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0)) : FVector((DScale.X * XYSIZE) + XYSIZE, 0, ((Scale->Z >= 0) ? ZFLOORHALF : 0));
		// Walls
		XForm[0].Add(FTransform(FRotator(90, (DScale.Y > DScale.X) ? 90 : 0, 0), isLocation, FVector(zScale, 1, 1)));
		XForm[0].Add(FTransform(FRotator(90, (DScale.Y > DScale.X) ? 270 : 180, 0), isLocation * FVector(-1, -1, 1), FVector(zScale, 1, 1)));

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
		XForm[0].Add(FTransform(FRotator(180,0,0), isLocation * (FVector(-1, -1, 1)), iScale));
		XForm[0].Add(FTransform(FRotator(180,0,0), isLocation, iScale));
		isLocation = (DScale.Y > DScale.X) ? FVector(XYSIZE, (DScale.Y * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0)) : FVector((DScale.X * XYSIZE) + XYSIZE, XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0));
		// Walls in all four directions
		XForm[0].Add(FTransform(FRotator(90, (DScale.Y > DScale.X) ? 90 : 0, 0), isLocation, FVector(zScale, 1, 1)));
		XForm[0].Add(FTransform(FRotator(90, (DScale.Y > DScale.X) ? 90 : 0, 0), isLocation * FVector((DScale.Y > DScale.X) ? -1 : 1, (DScale.Y > DScale.X) ? 1 : -1, 1), FVector(zScale, 1, 1)));
		XForm[0].Add(FTransform(FRotator(90, (DScale.Y > DScale.X) ? 270 : 180, 0), isLocation * FVector(-1, -1, 1), FVector(zScale, 1, 1)));
		XForm[0].Add(FTransform(FRotator(90, (DScale.Y > DScale.X) ? 270 : 180, 0), isLocation * FVector((DScale.Y > DScale.X) ? 1 : -1, (DScale.Y > DScale.X) ? -1 : 1, 1), FVector(zScale, 1, 1)));
		// Trim Wedges
		isLocation = (DScale.Y > DScale.X) ? FVector(0, (DScale.Y * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? zStart : zCursor)) : FVector((DScale.X * XYSIZE) + XYSIZE, 0, ((Scale->Z >= 0) ? zStart : zCursor));
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
		isLocation = (DScale.Y > DScale.X) ? FVector(AxisOffset, 0, (Scale->Z >= 0) ? zStart : zCursor) : FVector(0, AxisOffset, (Scale->Z >= 0) ? zStart : zCursor);

		XForm[0].Add(FTransform(FRotator(0.0), isLocation, iScale));
		XForm[0].Add(FTransform(FRotator(0.0), isLocation * (FVector(-1, -1, 1)), iScale));
		isLocation.Z = (Scale->Z < 0) ? zStart : zCursor;
		XForm[0].Add(FTransform(FRotator(180,0,0), isLocation, iScale));
		XForm[0].Add(FTransform(FRotator(180,0,0), isLocation * (FVector(-1, -1, 1)), iScale));
		// Add Walls
		isLocation = (DScale.Y > DScale.X) ? FVector(AxisOffset, (iScale.Y * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0)) : FVector((iScale.X * XYSIZE) + XYSIZE, AxisOffset, ((Scale->Z >= 0) ? ZFLOORHALF : 0));
		if ((i - 1) <= 0)
		{
			//Add remaining walls needed to meet width of 5, could probably moved into just scaling the central wall instantiated above outside the for loop
			XForm[0].Add(FTransform(FRotator(90, (DScale.Y > DScale.X) ? 90 : 0, 0), isLocation, FVector(zScale, 1, 1)));
			XForm[0].Add(FTransform(FRotator(90, (DScale.Y > DScale.X) ? 90 : 0, 0), isLocation * FVector((DScale.Y > DScale.X) ? -1 : 1, (DScale.Y > DScale.X) ? 1 : -1, 1), FVector(zScale, 1, 1)));
			XForm[0].Add(FTransform(FRotator(90, (DScale.Y > DScale.X) ? 270 : 180, 0), isLocation * FVector((DScale.Y > DScale.X) ? 1 : -1, (DScale.Y > DScale.X) ? -1 : 1, 1), FVector(zScale, 1, 1)));
			XForm[0].Add(FTransform(FRotator(90, (DScale.Y > DScale.X) ? 270 : 180, 0), isLocation * FVector(-1, -1, 1), FVector(zScale, 1, 1)));
		}
		else
		{
			

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
	
	float isv = (DScale.Y > DScale.X) ? fmax(roundf((DScale.X / 2) - 2), 1) : fmax(roundf((DScale.Y / 2) - 2),1);
	iScale = FVector(fmax(isv - 1,1),zScale,fmax(isv-1,1));
	float xsv = (DScale.Y > DScale.X) ? (DScale.X) : (DScale.Y) ;
	
	
	float ysv = (DScale.Y > DScale.X) ? (DScale.Y) : (DScale.X ) ;
	
	
	xsv /= 2;
	ysv /= 2;
	//float omg = (fmax(isv - 2, 0) / 2);
	float omg = (isv - 2) / 2;
	xsv -= omg;
	ysv -= omg;
	ysv *= 100;
	xsv *= 100;
	float zoff = (Scale->Z >= 0) ? 250 : 0;
	isLocation = (DScale.Y > DScale.X) ? FVector(xsv, ysv, zoff) : FVector(ysv, xsv, zoff);
	XForm[1].Add(FTransform(FRotator(0, 90, 90), isLocation, iScale));
	XForm[1].Add(FTransform(FRotator(0, 180, 90), isLocation * FVector(-1, 1, 1), iScale));
	XForm[1].Add(FTransform(FRotator(0, 0, 90), isLocation * FVector(1, -1, 1), iScale));
	XForm[1].Add(FTransform(FRotator(0, 270, 90), isLocation * FVector(-1, -1, 1), iScale));

	iScale = (DScale.Y > DScale.X) ? FVector(1, VScale, 1) : FVector(VScale, 1, 1);
	isLocation = (DScale.Y > DScale.X) ? FVector(AxisOffset, (iScale.Y * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0)) : FVector((iScale.X * XYSIZE) + XYSIZE, AxisOffset, ((Scale->Z >= 0) ? ZFLOORHALF : 0));
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
	iScale = (DScale.Y > DScale.X) ? FVector(zScale, iScale.Y, 1) : FVector(zScale, iScale.X, 1);
	
	XForm[0].Add(FTransform(FRotator(90, ((DScale.Y > DScale.X) ? 0 : 90), 0), isLocation, iScale));
	XForm[0].Add(FTransform(FRotator(90, ((DScale.Y > DScale.X) ? 180 : 270), 0), isLocation * FVector(-1, -1, 1), iScale));

	// Add Wedges above and below Ending Walls
	isLocation.Z = (Scale->Z >= 0) ? zStart : zCursor;
	XForm[1].Add(FTransform(FRotator(0, ((DScale.Y > DScale.X) ? 0 : 90), 0), isLocation, FVector(1, iScale.Y, 1)));
	XForm[1].Add(FTransform(FRotator(0, ((DScale.Y > DScale.X) ? 180 : 270), 0), isLocation * FVector(-1, -1, 1), FVector(1, iScale.Y, 1)));
	isLocation.Z = (Scale->Z >= 0) ? zCursor : zStart;
	XForm[1].Add(FTransform(FRotator(180, ((DScale.Y > DScale.X) ? 0 : 90), 0), isLocation * FVector(-1, -1, 1), FVector(1, iScale.Y, 1)));
	XForm[1].Add(FTransform(FRotator(180, ((DScale.Y > DScale.X) ? 180 : 270), 0), isLocation, FVector(1, iScale.Y, 1)));
}


void DragTemplate::Sphere(TArray<FTransform> * XForm, FVector * Scale, int bias)
{
	// Size Controls
	int minSize = 5;

	//Get Z sign
	float signZ = (Scale->Z >= 0) ? signZ = 1 : signZ = -1;
	// Calculate the drag scale from the extent scale that is passed in, this is used to scale for each HISM instance, only XY is captured in this.
	FVector DScale = *Scale / XYSIZE; DScale = DScale.GetAbs(); DScale.X = fmax(DScale.X - 1, 1); DScale.Y = fmax(DScale.Y - 1, 1); DScale.Z = abs(Scale->Z); DScale.Z /= ZFLOORHALF;
	// Get the Z start and end locations in world space
	float zCursor = DScale.Z * signZ; zCursor *= ZFLOORHALF; zCursor += (Scale->Z >= 0) ? ZFLOORSIZE : 0;
	float zStart = DScale.Z * signZ; zStart *= -ZFLOORHALF;
	// Get the Z drag scale
	float zScale = DScale.Z; zScale *= ZFLOORSCALE; zScale += (Scale->Z >= 0) ? (ZFLOORSCALE - 1) : -1;

	FVector isLocation;

	//Get Scaler based on greater drag direction
	float iScaler = (DScale.Y > DScale.X) ? DScale.X : DScale.Y;

	//Allowed number of slants X,Y,Z
	int sLoop = (DScale.X > DScale.Y) ? fmax(((DScale.Y - minSize) / 2),0) : fmax(((DScale.X - minSize) / 2),0);
	sLoop = fmin(sLoop, (1 + (roundf(fmax(((DScale.Z - ((Scale->Z >= 0) ? 1 : 2)) / 2), 0)) * 4)));
	sLoop = (bias >= 0) ? fmin(bias, sLoop) : sLoop;

	FVector iScale = FVector(DScale.X - (sLoop * 2), DScale.Y - (sLoop * 2), 1);
	iScale.GridSnap(0.5);

	//Floor and ceiling
	XForm[0].Add(FTransform(FRotator((Scale->Z >= 0) ? 0 : 180,0,0), FVector(0, 0, zStart), iScale));
	XForm[0].Add(FTransform(FRotator((Scale->Z >= 0) ? 180 : 0,0,0), FVector(0, 0, zCursor), iScale));

	// Walls
	iScale = (DScale.Y > DScale.X) ? FVector((zScale - (sLoop * 2)), DScale.Y - (sLoop * 2), 1) : FVector(DScale.X - (sLoop * 2), (zScale - (sLoop * 2)), 1);
	iScale = iScale.GetAbs();
	isLocation = (DScale.Y > DScale.X) ? FVector((DScale.X * XYSIZE) + XYSIZE, 0, ((Scale->Z >= 0) ? ZFLOORHALF : 0)) : FVector(0,(DScale.Y * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0));
	isLocation.GridSnap(XYSIZE);
	XForm[0].Add(FTransform(FRotator((DScale.Y > DScale.X) ? 90 : 0, 0, (DScale.Y > DScale.X) ? 0 : 270), isLocation, iScale));
	XForm[0].Add(FTransform(FRotator((DScale.Y > DScale.X) ? 270 : 0, 0, (DScale.Y > DScale.X) ? 0 : 90), isLocation * FVector(-1,-1,1), iScale));

	//Wedge from Wall to Ceil/Floor 
	iScale = (DScale.Y > DScale.X) ? FVector(sLoop + 1 , DScale.Y - (sLoop * 2), sLoop + 1) : FVector(sLoop + 1, DScale.X - (sLoop * 2), sLoop + 1);
	iScale.GridSnap(0.5);
	isLocation.Z = ((Scale->Z >= 0) ? (ZFLOORHALF + ((((zScale - (sLoop )) * XYSIZE) + XYSIZE))) : (((((zScale - (sLoop )) * XYSIZE) + XYSIZE))));
	(DScale.Y > DScale.X) ? isLocation.X = ((DScale.X - sLoop)* XYSIZE) + XYSIZE : isLocation.Y = ((DScale.Y - sLoop) * XYSIZE) + XYSIZE;
	XForm[1].Add(FTransform(FRotator(180, ((DScale.Y > DScale.X) ? 180 : 270), 0), isLocation, iScale));
	XForm[1].Add(FTransform(FRotator(180, ((DScale.Y > DScale.X) ? 0 : 90), 0), isLocation * FVector(-1, -1, 1), iScale));
	isLocation.Z = (Scale->Z >= 0) ? ZFLOORHALF - ((((zScale - (sLoop)) * XYSIZE) + XYSIZE)) : -((((zScale - (sLoop)) * XYSIZE) + XYSIZE));
	XForm[1].Add(FTransform(FRotator(0, ((DScale.Y > DScale.X) ? 0 : 90), 0), isLocation, iScale));
	XForm[1].Add(FTransform(FRotator(0, ((DScale.Y > DScale.X) ? 180 : 270), 0), isLocation * FVector(-1, -1, 1), iScale));
	
	// Walls
	iScale = (DScale.Y > DScale.X) ? FVector(DScale.X - (sLoop * 2), (zScale - (sLoop * 2)), 1) : FVector((zScale - (sLoop * 2)), DScale.Y - (sLoop * 2), 1);
	iScale.GridSnap(0.5);
	isLocation = (DScale.Y > DScale.X) ? FVector(0, (DScale.Y * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0)) : FVector((DScale.X * XYSIZE) + XYSIZE, 0, ((Scale->Z >= 0) ? ZFLOORHALF : 0));
	isLocation.GridSnap(XYSIZE);
	XForm[0].Add(FTransform(FRotator((DScale.Y > DScale.X) ? 0 : 90, 0, (DScale.Y > DScale.X) ? 270 : 0), isLocation, iScale));
	XForm[0].Add(FTransform(FRotator((DScale.Y > DScale.X) ? 0 : 270, 0, (DScale.Y > DScale.X) ? 90 : 0), isLocation * FVector(-1, -1, 1), (iScale * FVector(-1, -1, 1).GetAbs())));

	//Wedge from Wall to Ceil/Floor 
	iScale = (DScale.Y > DScale.X) ? FVector(sLoop + 1, DScale.X - (sLoop * 2), sLoop + 1) : FVector(sLoop + 1, DScale.Y - (sLoop * 2), sLoop + 1);
	iScale.GridSnap(0.5);
	isLocation.Z = ((Scale->Z >= 0) ? (ZFLOORHALF + ((((zScale - (sLoop)) * XYSIZE) + XYSIZE))) : (((((zScale - (sLoop)) * XYSIZE) + XYSIZE))));
	(DScale.Y > DScale.X) ? isLocation.Y = ((DScale.Y - sLoop)* XYSIZE) + XYSIZE : isLocation.X = ((DScale.X - sLoop) * XYSIZE) + XYSIZE;
	XForm[1].Add(FTransform(FRotator(180, ((DScale.Y > DScale.X) ? 270 : 180), 0), isLocation, iScale));
	XForm[1].Add(FTransform(FRotator(180, ((DScale.Y > DScale.X) ? 90 : 0), 0), isLocation * FVector(-1, -1, 1), iScale));
	isLocation.Z = (Scale->Z >= 0) ? ZFLOORHALF - ((((zScale - (sLoop)) * XYSIZE) + XYSIZE)) : -((((zScale - (sLoop)) * XYSIZE) + XYSIZE));
	XForm[1].Add(FTransform(FRotator(0, ((DScale.Y > DScale.X) ? 90 : 0), 0), isLocation, iScale));
	XForm[1].Add(FTransform(FRotator(0, ((DScale.Y > DScale.X) ? 270 : 180), 0), isLocation * FVector(-1, -1, 1), iScale));

	//Wall to wall wedge
	iScale = FVector(sLoop + 1, (zScale - (sLoop * 2)), sLoop + 1);
	iScale.GridSnap(0.5);
	isLocation = FVector(((DScale.X - sLoop) * XYSIZE) + XYSIZE, ((DScale.Y - sLoop) * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0));
	isLocation.GridSnap(XYSIZE);
	isLocation.Z = ((Scale->Z >= 0) ? ZFLOORHALF : 0);
	XForm[1].Add(FTransform(FRotator(0, 90, 90), isLocation, iScale));
	XForm[1].Add(FTransform(FRotator(0, 270, 90), isLocation  * FVector(-1, -1, 1), iScale));
	XForm[1].Add(FTransform(FRotator(0, 0, 90), isLocation  * FVector(1, -1, 1), iScale));
	XForm[1].Add(FTransform(FRotator(0, 180, 90), isLocation  * FVector(-1, 1, 1), iScale));

	// Corners
	iScale = FVector(sLoop + 1, sLoop + 1, sLoop + 1);
	iScale.GridSnap(0.5);
	isLocation = FVector(((DScale.X - sLoop) * XYSIZE) + XYSIZE, ((DScale.Y - sLoop) * XYSIZE) + XYSIZE, ((Scale->Z >= 0) ? ZFLOORHALF : 0));
	isLocation.GridSnap(XYSIZE);
	isLocation.Z = (Scale->Z >= 0) ? ZFLOORHALF + (((zScale - ((sLoop + 1))) * XYSIZE)) : (((zScale - ((sLoop + 1))) * XYSIZE));
	XForm[2].Add(FTransform(FRotator(180, 270, 0), isLocation + FVector(0, 0, WORLDUNITSIZE), iScale));
	XForm[2].Add(FTransform(FRotator(180, 90, 0), isLocation * FVector(-1, -1, 1) + FVector(0, 0, WORLDUNITSIZE), iScale));
	XForm[2].Add(FTransform(FRotator(180, 180, 0), isLocation * FVector(1, -1, 1) + FVector(0, 0, WORLDUNITSIZE),iScale));
	XForm[2].Add(FTransform(FRotator(180, 0, 0), isLocation * FVector(-1, 1, 1) + FVector(0, 0, WORLDUNITSIZE), iScale));

	isLocation.Z = (Scale->Z >= 0) ? ZFLOORHALF - (((zScale - ((sLoop + 1))) * XYSIZE)) : -(((zScale - ((sLoop + 1))) * XYSIZE));
	XForm[2].Add(FTransform(FRotator(0, 0, 0), isLocation + FVector(0, 0, -WORLDUNITSIZE), iScale));
	XForm[2].Add(FTransform(FRotator(0, 180, 0), isLocation * FVector(-1, -1, 1) + FVector(0, 0, -WORLDUNITSIZE), iScale));
	XForm[2].Add(FTransform(FRotator(0, 270, 0), isLocation * FVector(1, -1, 1) + FVector(0, 0, -WORLDUNITSIZE), iScale));
	XForm[2].Add(FTransform(FRotator(0, 90, 0), isLocation * FVector(-1, 1, 1) + FVector(0, 0, -WORLDUNITSIZE), iScale));
}
