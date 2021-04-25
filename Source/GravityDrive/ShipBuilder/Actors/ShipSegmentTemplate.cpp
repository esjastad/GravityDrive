// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipSegmentTemplate.h"
#include "../../GDPlayerController.h"
#include "Kismet/KismetMathLibrary.h"

AShipSegmentTemplate::AShipSegmentTemplate() : AShipSegment()
{
	//Set Drag Type default
	CurrentType = SegmentType::cube;

	// Setup Guide Info
	Guide = CreateDefaultSubobject<USceneComponent>("Guide");
	Guide->SetupAttachment(RootComponent);
	XLength = CreateDefaultSubobject<UStaticMeshComponent>("XG");
	XLength->SetupAttachment(Guide);
	YLength = CreateDefaultSubobject<UStaticMeshComponent>("YG");
	YLength->SetupAttachment(Guide);
	ZLength = CreateDefaultSubobject<UStaticMeshComponent>("ZG");
	ZLength->SetupAttachment(Guide);

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
	
	// Set Guides
	Material = UMaterialInstanceDynamic::Create(MatGuide, this);
	XLength->SetStaticMesh(Cube);
	Material->SetVectorParameterValue("Color", FColor::Red);
	XLength->SetMaterial(0, Material);
	Material = UMaterialInstanceDynamic::Create(MatGuide, this);
	YLength->SetStaticMesh(Cube);
	Material->SetVectorParameterValue("Color", FColor::Green);
	YLength->SetMaterial(0, Material);
	Material = UMaterialInstanceDynamic::Create(MatGuide, this);
	ZLength->SetStaticMesh(Cube);
	Material->SetVectorParameterValue("Color", FColor::Blue);
	ZLength->SetMaterial(0, Material);
	XLength->bRenderCustomDepth = true;
	YLength->bRenderCustomDepth = true;
	ZLength->bRenderCustomDepth = true;
	XLength->CustomDepthStencilValue = 1;
	YLength->CustomDepthStencilValue = 2;
	ZLength->CustomDepthStencilValue = 3;

	// Set HISM meshes
	Material = UMaterialInstanceDynamic::Create(Cube->GetMaterial(0), this);
	HISMList[0]->SetStaticMesh(Cube);
	HISMList[1]->SetStaticMesh(Wedge);
	HISMList[2]->SetStaticMesh(Corner);
	HISMList[3]->SetStaticMesh(CubeAngle);
	HISMList[4]->SetStaticMesh(Angled);

	for (int i = 0; i < HISMList.Num(); ++i)
	{
		HISMList[i]->CastShadow = false;
		HISMList[i]->SetMaterial(0, Material);
	}
}

void AShipSegmentTemplate::PackageDrag(ADragData * Data)
{
	//This function should be setup to process a location per frame rate or tick, this will increase responsiveness but cause a delay for data to be ready for building
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
			FVector tLoc = tTransform.GetLocation();// -(tScale * XYSIZE);
			FVector subloc;
			FRotator rotation = FRotator(tTransform.GetRotation());
			rotation = rotation.GridSnap(FRotator(1));
			// Iterate across the x,y,z scale to get each stamp location
			switch (i)
			{
				case 0:
				{
					tLoc -= tScale * XYSIZE;
					for (int x = 0; x < tScale.X; ++x)
					{
						for (int y = 0; y < tScale.Y; ++y)
						{
							for (int z = 0; z < tScale.Z; ++z)
							{
								subloc = tLoc + FVector(x * WORLDUNITSIZE, y * WORLDUNITSIZE, z * WORLDUNITSIZE);
								subloc = subloc.GridSnap(WORLDUNITSIZE);
								tTransform.SetLocation(subloc);
								PieceToAdd.Transform = FTransform(tTransform.GetRotation(), tTransform.GetLocation(), FVector(1));
								Data->PieceMap.Add(tTransform.GetLocation(), PieceToAdd);
							}
						}
					}
				}
				break;
				case 1:
				{
					int temp = int(abs(rotation.Yaw + 360)) % 360;
					float xs = (abs(rotation.Roll) == 0) ? ((temp == 180 || temp == 90) ? 1 : -1) : ((temp == 180 || temp == 90) ? -1 : 1);
					float ys = (abs(rotation.Roll) == 0) ? ((temp == 180 || temp == 270) ? 1 : -1) : ((temp == 180 || temp == 270) ? -1 : 1);

					subloc = FVector(tLoc.X + ((tScale.X - 1) * XYSIZE * xs), tLoc.Y + ((tScale.Y - 1) * XYSIZE * ys), tLoc.Z - ((tScale.Z - 1) * XYSIZE));

					xs *= -1;
					ys *= -1;

					tLoc = subloc;
					temp = temp % 180;

					int innerloop = (temp == 90) ? tScale.X : tScale.Y;
					FVector additive = (abs(rotation.Roll) == 90) ? FVector(WORLDUNITSIZE * xs, WORLDUNITSIZE * ys, 0) : (temp == 90) ? FVector(WORLDUNITSIZE * xs, 0, 0) : FVector(0, WORLDUNITSIZE * ys, 0);
					FVector offset = (abs(rotation.Roll) == 90) ? FVector(0, 0, WORLDUNITSIZE) : (temp == 90) ? FVector(0, WORLDUNITSIZE * ys, WORLDUNITSIZE) : FVector(WORLDUNITSIZE * xs, 0, WORLDUNITSIZE);

					for (int z = 0; z < tScale.Z; ++z)
					{
						subloc = tLoc + (offset * z);
						subloc = subloc.GridSnap(XYSIZE);
						for (int x = 0; x < innerloop; ++x)
						{
							tTransform.SetLocation(subloc);
							PieceToAdd.StampType = AStampType::stampWedge;
							PieceToAdd.Transform = FTransform(tTransform.GetRotation(), tTransform.GetLocation(), FVector(1));
							Data->PieceMap.Add(tTransform.GetLocation(), PieceToAdd);
							if ((abs(rotation.Roll) == 90) ? (x < (innerloop - 1)) : ((abs(rotation.Roll) == 0) ? (z < tScale.Z - 1) : z > 0))
							{ //Inner wedge stamp
								PieceToAdd.StampType = AStampType::stampInvertedWedge;
								FVector vec = (abs(rotation.Roll) == 90) ? ((temp == 0) ? tTransform.GetLocation() + FVector(WORLDUNITSIZE * xs, 0, 0) : (tTransform.GetLocation() + FVector(0, WORLDUNITSIZE * ys, 0))) : ((abs(rotation.Roll) == 0) ? tTransform.GetLocation() + FVector(0, 0, WORLDUNITSIZE) : tTransform.GetLocation() + FVector(0, 0, -WORLDUNITSIZE));
								FRotator rot = (abs(rotation.Roll) == 90) ? (FRotator(tTransform.GetRotation()) + FRotator(0, 180, 0)) : ((abs(rotation.Roll) == 0) ? FRotator(tTransform.GetRotation()) + FRotator(90, 180, 0) : FRotator(tTransform.GetRotation()) + FRotator(180, 0, 0));
								PieceToAdd.Transform = FTransform(rot, vec, FVector(1));
								Data->PieceMap.Add(vec, PieceToAdd);
							}

							subloc += additive;
							subloc = subloc.GridSnap(XYSIZE);
						}
					}
				}
				break;
				case 2:
				{
					int temp = int(abs(rotation.Yaw + 360)) % 360;
					float xs = (abs(rotation.Roll) == 0) ? ((temp == 180 || temp == 270) ? 1 : -1) : ((temp == 180 || temp == 90) ? -1 : 1);
					float ys = (abs(rotation.Roll) == 0) ? ((temp == 0 || temp == 270) ? 1 : -1) : ((temp == 180 || temp == 270) ? -1 : 1);
					float zs = (abs(rotation.Roll) == 0) ? 1 : -1;
					subloc = FVector(tLoc.X + ((tScale.X - 1) * XYSIZE * xs), tLoc.Y + ((tScale.Y - 1) * XYSIZE * ys), tLoc.Z + ((tScale.Z - 1) * XYSIZE * zs));

					xs *= -1;
					ys *= -1;
					zs *= -1;

					tLoc = subloc;
					temp = temp % 180;

					int innerloop = (temp == 90) ? tScale.X : tScale.Y;
					FVector additive = FVector(xs, ys, 0) * WORLDUNITSIZE;
					FVector offset = (abs(rotation.Roll) == 0) ? ((temp == 90) ? FVector(xs, 0, zs) * WORLDUNITSIZE : FVector(0, ys, zs) * WORLDUNITSIZE) : ((temp == 90) ? FVector(0, ys, zs) * WORLDUNITSIZE : FVector(xs, 0, zs) * WORLDUNITSIZE);

					for (int z = 0; z < tScale.Z; ++z)
					{
						subloc = tLoc + (offset * z);
						subloc = subloc.GridSnap(XYSIZE);
						for (int x = 0; x < innerloop - z; ++x)
						{
							tTransform.SetLocation(subloc);
							PieceToAdd.Transform = FTransform(tTransform.GetRotation(), tTransform.GetLocation(), FVector(1));
							PieceToAdd.StampType = AStampType::stampCorner;
							Data->PieceMap.Add(tTransform.GetLocation(), PieceToAdd);
							if (z < tScale.Z - 1 && x > 0 && x < innerloop -z -1) //((abs(rotation.Roll) == 90) ? (x < (innerloop - 1)) : ((abs(rotation.Roll) == 0) ? (z < tScale.Z - 1) : z > 0))
							{ //Inner Corner stamp
								FVector vec = (temp == 0) ? tTransform.GetLocation() + FVector(-WORLDUNITSIZE * xs,WORLDUNITSIZE * ys,0) : tTransform.GetLocation() + FVector(WORLDUNITSIZE * xs, -WORLDUNITSIZE * ys, 0);
								vec = (abs(rotation.Roll) == 0) ? vec : ((temp == 0) ? tTransform.GetLocation() + FVector(WORLDUNITSIZE * xs, -WORLDUNITSIZE * ys, 0) : tTransform.GetLocation() + FVector(-WORLDUNITSIZE * xs, WORLDUNITSIZE * ys, 0));
								FRotator rot = FRotator(tTransform.GetRotation()) + FRotator(0, (abs(rotation.Roll) == 0) ? 270 : 90, 180);
								PieceToAdd.StampType = AStampType::stampInvertedCorner;
								PieceToAdd.Transform = FTransform(rot, vec, FVector(1));
								Data->PieceMap.Add(vec, PieceToAdd);
							}
							if (z < tScale.Z - 1 && x > 0 && x < innerloop - z)
							{
								FVector vec = (temp == 90) ? tTransform.GetLocation() + FVector(-WORLDUNITSIZE * xs, 0, 0) : tTransform.GetLocation() + FVector(0, -WORLDUNITSIZE * ys, 0);
								vec = (abs(rotation.Roll) == 180) ? vec : (temp == 90) ? tTransform.GetLocation() + FVector(0, -WORLDUNITSIZE * ys, 0) : tTransform.GetLocation() + FVector(-WORLDUNITSIZE * xs, 0, 0);
								FRotator rot = FRotator(tTransform.GetRotation());
								PieceToAdd.StampType = AStampType::stampAngled;
								PieceToAdd.Transform = FTransform(rot, vec, FVector(1));
								Data->PieceMap.Add(vec, PieceToAdd);
							}

							subloc += additive;
							subloc = subloc.GridSnap(XYSIZE);
						}
					}
				
				}
				break;

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
	CurrentType = type;
}

void AShipSegmentTemplate::Update(FVector Location, FVector Scale, bool Valid)
{
	AGDPlayerController * PC = Cast<AGDPlayerController>(GetWorld()->GetFirstPlayerController());
	FVector GSign = -Scale.GetSignVector();
	FVector CLoc = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	FVector UIScale = Scale.GetAbs() / XYSIZE;
	FVector WCenter = Location + Scale;
	FVector CDIFF = CLoc - WCenter;
	float GDist;
	CDIFF.ToDirectionAndLength(CLoc, GDist);
	GDist /= 32000;
	FVector Xoffset = FVector((Scale.X * -0.5 * fmax(fmin(GDist, 2), 1)) + (WORLDUNITSIZE * GSign.X),(WORLDUNITSIZE * -GSign.Y),0);
	FVector Yoffset = FVector((WORLDUNITSIZE * -GSign.X), (Scale.Y * -0.5 * fmax(fmin(GDist, 2), 1)) + (WORLDUNITSIZE * GSign.Y), 0);
	FVector Zoffset = FVector((WORLDUNITSIZE * -GSign.X), (WORLDUNITSIZE * -GSign.Y), (Scale.Z * 0.5 * fmax(fmin(GDist, 2), 1)) + 500);
	FVector DDDScale = FVector(UIScale.X + 1, UIScale.Y + 1, (UIScale.Z / 5) + 1);
	FVector2D UICenter;
	FVector2D Xoff;
	FVector2D Yoff;
	FVector2D Zoff;
	

	PC->ProjectWorldLocationToScreen(WCenter, UICenter);
	PC->ProjectWorldLocationToScreen(WCenter + Xoffset, Xoff);
	PC->ProjectWorldLocationToScreen(WCenter + Yoffset, Yoff);
	PC->ProjectWorldLocationToScreen(WCenter + Zoffset, Zoff);
	if (PC) PC->BuildHud->DrawDragDimensions(DDDScale, UICenter, Xoff, Yoff, Zoff);

	// Update Guide
	FVector GLoc = Location + Scale + FVector(0, 0, 0);
	
	
	FRotator CRotation = UKismetMathLibrary::FindLookAtRotation(Location, CLoc);
	
	CLoc -= Location;
	
	CLoc.ToDirectionAndLength(CLoc, GDist);
	GDist *= 0.01;
	GDist = fmin(fmax(GDist * 10, 200), 3000);
	GSign.Z = (Scale.Z >= 0) ? 1 : -1;
	Guide->SetWorldLocation(GLoc);
	GLoc = Scale * -1;
	GLoc.Z += (roundf(GLoc.Z) == 0) ? 250 : (50 * -GSign.Z);
	XLength->SetRelativeLocation(FVector(GLoc.X + (XYSIZE * GSign.X),0,0));
	XLength->SetWorldScale3D(FVector(Scale.X / XYSIZE,1,1));
	YLength->SetRelativeLocation(FVector(0, GLoc.Y + (XYSIZE * GSign.Y), 0));
	YLength->SetWorldScale3D(FVector(1, Scale.Y / XYSIZE, 1));
	ZLength->SetRelativeLocation(FVector(0, 0, GLoc.Z + (XYSIZE * GSign.Z)));
	ZLength->SetWorldScale3D(FVector(1, 1, GLoc.Z / XYSIZE));
	

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

	(Valid) ? Material->SetVectorParameterValue("Color",FColor::Green) : Material->SetVectorParameterValue("Color", FColor::Red);
	
}

void AShipSegmentTemplate::Destroyed()
{
	AGDPlayerController * PC = Cast<AGDPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC) PC->BuildHud->RemoveDragDimensions();
}

// Fill in the respective transforms
void AShipSegmentTemplate::FillTransform(TArray<FTransform> * XForm, FVector * Scale)
{
	FVector DScale = *Scale / XYSIZE; DScale = DScale.GetAbs(); DScale.X = fmax(DScale.X - 1, 1); DScale.Y = fmax(DScale.Y - 1, 1); DScale.Z = abs(Scale->Z); DScale.Z /= ZFLOORHALF;
	float extentZ = (Scale->Z >= 0) ? (ZSIZE * (DScale.Z + 1)) : (ZSIZE * DScale.Z);
	DragData.Extent = FVector(DScale.X + 1, DScale.Y + 1, extentZ) * XYSIZE;

	switch (CurrentType) {
		case cube:
			DragTemplate::Cube(XForm, Scale, 0);
			break;
		case cylinder:
			DragTemplate::Cylinder(XForm, Scale, 0);
			break;
		case sphere:
			DragTemplate::Sphere(XForm, Scale, -1);
			break;
	}
}