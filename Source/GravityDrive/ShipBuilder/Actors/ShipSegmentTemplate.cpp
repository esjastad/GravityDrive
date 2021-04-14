// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipSegmentTemplate.h"

AShipSegmentTemplate::AShipSegmentTemplate() : AShipSegment()
{
	CurrentType = SegmentType::cube;

	//Add HISMS and attach to RootComponent
	UHierarchicalInstancedStaticMeshComponent * NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Cube");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Wedge");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Corner");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM); 
}

// Called when the game starts or when spawned
void AShipSegmentTemplate::BeginPlay()
{
	Super::BeginPlay();
	//Set HISM meshes
	HISMList[0]->SetStaticMesh(Cube);
	HISMList[1]->SetStaticMesh(Wedge);
	HISMList[2]->SetStaticMesh(Corner);
	
}

void AShipSegmentTemplate::PackageDrag(ADragData * Data)
{
	int olimit = HISMList.Num();
	for (int i = 0; i < olimit; ++i)
	{	
		int ilimit = HISMList[i]->GetInstanceCount();
		for (int j = 0; j < ilimit; ++j)
		{
			// Get the current instance transform
			FTransform tTransform;
			// Create a segment piece and set the stamp type
			APieceData PieceToAdd;
			PieceToAdd.StampType = (i > 0) ? (i > 1) ? AStampType::stampCorner : AStampType::stampWedge : AStampType::stampCube;
			HISMList[i]->GetInstanceTransform(j, tTransform, true);
			//Add HISMData
			PieceToAdd.Transform = FTransform(tTransform.GetRotation(), tTransform.GetLocation(), tTransform.GetScale3D());
			Data->HISMData.Add(tTransform.GetLocation(), PieceToAdd);
			// Get the rotated scale, this way the scale matches the world direction
			FVector tScale = FVector(tTransform.GetRotation().RotateVector(tTransform.GetScale3D())).GetAbs().GridSnap(1);
			// This location is the starting point to offset from
			FVector tLoc = tTransform.GetLocation() - (tScale * XYSIZE);
			
			
			//Iterate across the x,y,z scale to get each stamp location
			for (int x = 0; x < tScale.X; ++x)
			{
				for (int y = 0; y < tScale.Y; ++y)
				{
					for (int z = 0; z < tScale.Z; ++z)
					{
						FVector subloc = tLoc + FVector(x * WORLDUNITSIZE, y * WORLDUNITSIZE, z * WORLDUNITSIZE);
						subloc = subloc.GridSnap(WORLDUNITSIZE);
						tTransform.SetLocation(subloc);
						PieceToAdd.Transform = FTransform(tTransform.GetRotation(), tTransform.GetLocation(), FVector(1));
						Data->PieceMap.Add(tTransform.GetLocation(), PieceToAdd);
					}
				}
			}	
		}
	}
	// Set the location and extent data of the drag
	Data->Extent = FVector(DragData.Extent.X, DragData.Extent.Y, DragData.Extent.Z);
	Data->Location = FVector(DragData.Location.X, DragData.Location.Y, DragData.Location.Z);
}

// Initialize type enum
void AShipSegmentTemplate::Initialize(SegmentType type)
{
	CurrentType = SegmentType::cylinder;
}

void AShipSegmentTemplate::Update(FVector Location, FVector Scale)
{
	TArray<FTransform> Transforms[3];
	//Update this actors location
	SetActorLocation(Location);

	float locZ = Location.Z; locZ += (Scale.Z >= 0) ? ZFLOORHALF : 0;
	DragData.Location = FVector(Location.X, Location.Y, locZ);
	
	//Get cube HISM transforms
	FillTransform(Transforms, &Scale);

	//Update HISM transforms to match drag
	for (int i = 0; i < 3; ++i) {
		HISMList[i]->ClearInstances();
		HISMList[i]->AddInstances(Transforms[i], false);
	}
	
}

// Fill in the respective transforms
void AShipSegmentTemplate::FillTransform(TArray<FTransform> * XForm, FVector * Scale)
{
	// Currently setup to use cube only

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
	// Get the drag data extent in world units
	float extentZ = (Scale->Z >= 0) ? (ZSIZE * (DScale.Z + 1)) : (ZSIZE * DScale.Z);
	// Set drag data extent member variable
	DragData.Extent = FVector(DScale.X + 1, DScale.Y + 1, extentZ) * XYSIZE;

	//Used in Cylinder case only
	float AxisOffset;
	int LoopLimit;
	float iScaler = (DScale.Y > DScale.X) ? DScale.X : DScale.Y;
	FVector iScale = (DScale.Y > DScale.X) ? FVector(1, DScale.Y, 1) : FVector(DScale.X, 1, 1);
	FVector isLocation = (DScale.Y > DScale.X) ? FVector(XYSIZE, 0, zStart) : FVector(0, XYSIZE, zStart);

	switch (CurrentType) {
		case cube:
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
			break;
		case cylinder:
			// If the drag scale is odd
			if (int(iScaler) % 2)
			{
				XForm[0].Add(FTransform(FRotator(0.0), FVector(0, 0, zStart), iScale));
				XForm[0].Add(FTransform(FRotator(0.0), FVector(0, 0, zCursor), iScale));
				AxisOffset = WORLDUNITSIZE;
				LoopLimit = iScaler * 0.5;
			}
			else
			{
				XForm[0].Add(FTransform(FRotator(0.0), isLocation * (FVector(-1, -1, 1)), iScale));
				XForm[0].Add(FTransform(FRotator(0.0), isLocation, iScale));
				isLocation.Z = zCursor;
				XForm[0].Add(FTransform(FRotator(0.0), isLocation * (FVector(-1, -1, 1)), iScale));
				XForm[0].Add(FTransform(FRotator(0.0), isLocation, iScale));
				AxisOffset = WORLDUNITSIZE + XYSIZE;
				LoopLimit = (iScaler - 1) * 0.5;
			}

			for (int x = 0; x < LoopLimit; ++x)
			{
				int scale = fmax((x - 1), 0.0);
				scale = (DScale.Y > DScale.X) ? (DScale.Y - (scale * 2)) : (DScale.X - (scale * 2));

				scale = (scale < 5) ? 5 : scale;
				iScale = (DScale.Y > DScale.X) ? FVector(1, scale, 1) : FVector(scale, 1, 1);
				isLocation = (DScale.Y > DScale.X) ? FVector(AxisOffset, 0, zStart) : FVector(0, AxisOffset, zStart);

				XForm[0].Add(FTransform(FRotator(0.0), isLocation, iScale));
				XForm[0].Add(FTransform(FRotator(0.0), isLocation * (FVector(-1, -1, 1)), iScale));
				isLocation.Z = zCursor;
				XForm[0].Add(FTransform(FRotator(0.0), isLocation, iScale));
				XForm[0].Add(FTransform(FRotator(0.0), isLocation * (FVector(-1, -1, 1)), iScale));
				AxisOffset += WORLDUNITSIZE;
			}
			break;
	}
}