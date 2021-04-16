// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipSegmentTemplate.h"

AShipSegmentTemplate::AShipSegmentTemplate() : AShipSegment()
{
	CurrentType = SegmentType::cube;

	// Add HISMS and attach to RootComponent
	UHierarchicalInstancedStaticMeshComponent * NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Cube");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Wedge");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Corner");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM); 

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("CubeAngle");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Angled");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);
}

// Called when the game starts or when spawned
void AShipSegmentTemplate::BeginPlay()
{
	Super::BeginPlay();
	// Set HISM meshes
	HISMList[0]->SetStaticMesh(Cube);
	HISMList[1]->SetStaticMesh(Wedge);
	HISMList[2]->SetStaticMesh(Corner);
	HISMList[3]->SetStaticMesh(CubeAngle);
	HISMList[4]->SetStaticMesh(Angled);
	
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
			PieceToAdd.StampType = (i > 0) ? (i > 1) ? (i > 2) ? (i > 3) ? AStampType::stampAngled : AStampType::stampCubeAngle : AStampType::stampCorner : AStampType::stampWedge : AStampType::stampCube;
			HISMList[i]->GetInstanceTransform(j, tTransform, true);
			// Add HISMData
			PieceToAdd.Transform = FTransform(tTransform.GetRotation(), tTransform.GetLocation(), tTransform.GetScale3D());
			Data->HISMData.Add(tTransform.GetLocation(), PieceToAdd);
			// Get the rotated scale, this way the scale matches the world direction
			FVector tScale = FVector(tTransform.GetRotation().RotateVector(tTransform.GetScale3D())).GetAbs().GridSnap(1);
			// This location is the starting point to offset from
			FVector tLoc = tTransform.GetLocation() - (tScale * XYSIZE);
			
			// Iterate across the x,y,z scale to get each stamp location
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
	CurrentType = SegmentType::sphere;
}

void AShipSegmentTemplate::Update(FVector Location, FVector Scale)
{
	TArray<FTransform> Transforms[5];
	// Update this actors location
	SetActorLocation(Location);

	float locZ = Location.Z; locZ += (Scale.Z >= 0) ? ZFLOORHALF : 0;
	DragData.Location = FVector(Location.X, Location.Y, locZ);
	
	// Get cube HISM transforms
	FillTransform(Transforms, &Scale);

	// Update HISM transforms to match drag
	for (int i = 0; i < HISMList.Num(); ++i) {
		HISMList[i]->ClearInstances();
		HISMList[i]->AddInstances(Transforms[i], false);
	}
	
}

// Fill in the respective transforms
void AShipSegmentTemplate::FillTransform(TArray<FTransform> * XForm, FVector * Scale)
{
	switch (CurrentType) {
		case cube:
			DragTemplate::Cube(XForm, Scale);
			break;
		case cylinder:
			DragTemplate::Cylinder(XForm, Scale);
			break;
		case sphere:
			/*if (abs((Scale->Z / (ZFLOORHALF))) > 1 && abs((Scale->X / XYSIZE)) > 11 && abs((Scale->Y / XYSIZE)) > 11)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("X and Y = %f , %f"), Scale->X, Scale->Y));*/
				DragTemplate::Sphere(XForm, Scale);
			/*}
			else
			{
				DragTemplate::Cylinder(XForm, Scale);
			}*/
			break;
	}
}