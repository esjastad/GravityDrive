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
			FTransform tTransform;
			HISMList[i]->GetInstanceTransform(j, tTransform, true);
			FVector tScale = tTransform.GetScale3D();
			FVector tLoc = tTransform.GetLocation() - (tScale * XYSIZE);
			
			APieceData PieceToAdd;
			PieceToAdd.StampType = (i > 0) ? (i > 1) ? AStampType::stampCorner : AStampType::stampWedge : AStampType::stampCube;
			
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
	Data->Extent = FVector(DragData.Extent.X, DragData.Extent.Y, DragData.Extent.Z);
	Data->Location = FVector(DragData.Location.X, DragData.Location.Y, DragData.Location.Z);
}

// Initialize type enum
void AShipSegmentTemplate::Initialize(SegmentType type)
{
	CurrentType = type;	
}

void AShipSegmentTemplate::Update(FVector Location, FVector Scale)
{
	TArray<FTransform> Transforms[3];
	//Update this actors location
	SetActorLocation(Location);
	float extentZ = Scale.Z;
	extentZ += (Scale.Z >= 0) ? ZFLOORHALF : 0;
	float locZ = Location.Z;
	locZ += (Scale.Z >= 0) ? ZFLOORHALF : 0;
	DragData.Extent = FVector(Scale.X, Scale.Y, extentZ);
	DragData.Location = FVector(Location.X, Location.Y, locZ);
	
	switch (CurrentType) {
		case cube:
			//Get cube HISM transforms
			FillTransform(Transforms, &Scale);

			//Update HISM transforms to match drag
			for (int i = 0; i < 3; ++i) {
				HISMList[i]->ClearInstances();
				HISMList[i]->AddInstances(Transforms[i], false);
			}
			break;

		case cylinder:
			
			break;
	}
}

// Fill in the respective transforms
void AShipSegmentTemplate::FillTransform(TArray<FTransform> * XForm, FVector * Scale)
{
	//Currently setup to use cube only
	float signX = (Scale->X >= 0) ? signX = -1 : signX = 1;
	float signY = (Scale->Y >= 0) ? signY = 1 : signY = -1;
	float signZ = (Scale->Z >= 0) ? signZ = 1 : signZ = -1;
	FVector DScale = *Scale / XYSIZE; DScale = DScale.GetAbs(); DScale.X = fmax(DScale.X - 1, 1); DScale.Y = fmax(DScale.Y - 1, 1); DScale.Z = abs(Scale->Z); DScale.Z /= ZFLOORHALF;

	float zCursor = DScale.Z * signZ; zCursor *= ZFLOORHALF; zCursor += (Scale->Z >= 0) ? ZFLOORSIZE : 0;

	float zStart = DScale.Z * signZ; zStart *= -ZFLOORHALF;

	float zScale = DScale.Z; zScale *= ZFLOORSCALE; zScale += (Scale->Z >= 0) ? (ZFLOORSCALE - 1) : -1;

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