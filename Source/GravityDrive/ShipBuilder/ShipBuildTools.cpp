// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipBuildTools.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Definitions.h"

// Sets default values, is default constructor
AShipBuildTools::AShipBuildTools()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// Create components
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	Cursor = CreateDefaultSubobject<UStaticMeshComponent>("Cursor");
	Grid = CreateDefaultSubobject<UStaticMeshComponent>("Grid");
	XAxis = CreateDefaultSubobject<UStaticMeshComponent>("XAxis");
	YAxis = CreateDefaultSubobject<UStaticMeshComponent>("YAxis");
	// Set attach heirarchy
	RootComponent = Scene;
	//Cursor->SetupAttachment(Scene);
	Cursor->SetWorldScale3D(FVector(1.01, 1.01, 1.01));
	Grid->SetupAttachment(RootComponent);
	XAxis->SetupAttachment(RootComponent);
	YAxis->SetupAttachment(RootComponent);

	//Set default state
	InitState();
	
}

// Called when the game starts or when spawned
void AShipBuildTools::BeginPlay()
{
	Super::BeginPlay();
	Grid->AddLocalOffset(FVector(0, 0, 51));
	Material = UMaterialInstanceDynamic::Create(Cursor->GetMaterial(0), this);
	Cursor->SetMaterial(0, Material);
	MPC = GetWorld()->GetParameterCollectionInstance(MPCAsset);
}

// Called every frame
void AShipBuildTools::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShipBuildTools::UpdateState(FVector* NewLoc, BuildMode CurrentMode, AItemType CurrentItem)
{
	Location = FVector(NewLoc->X, NewLoc->Y, NewLoc->Z);
	
	MPC->SetVectorParameterValue(FName("MouseLoc") , FLinearColor(*NewLoc));

	switch (CurrentMode)
	{
	case BuildMode::bmhull:
	{
		Cursor->SetStaticMesh(BuildC);
	}
	break;
	case BuildMode::bmitem:
	{
		Cursor->SetStaticMesh(WindowC);
	}
	break;
	}
}

void AShipBuildTools::InitState()
{
	Location.X = 0;
	Location.Y = 0;
	Location.Z = 0;

	ValidPlace = false;

	DragStart = FVector(ALTDEFAULT);
	GainingSegment = nullptr;
}

void AShipBuildTools::Update(FHitResult* NewHit, bool IsValid, BuildMode CurrentMode, AItemType CurrentItem)
{
	
	AActor * Parent = NewHit->Actor->GetAttachParentActor();	// Get Parent Reference
	FRotator CursorRotation(Parent->GetActorRotation());	// Get Parents rotation
	FVector * NewLoc(&NewHit->Location);					// Get the Hit Location
	
	Location = FVector(NewLoc->X, NewLoc->Y, NewLoc->Z);	// Set Class location variable
	CurrentItemType = CurrentItem;
	MPC->SetVectorParameterValue(FName("MouseLoc"), FLinearColor(*NewLoc));		//Update Material parameter

	switch (CurrentMode)
	{
	case BuildMode::bmhull:
	{
		Cursor->SetStaticMesh(BuildC);
		Cursor->SetWorldLocationAndRotation(NewHit->Location, CursorRotation);	// Update Cursor World loc and Rotation
	}
	break;
	case BuildMode::bmitem:
	{
		//FVector UnrotatedLoc = (UKismetMathLibrary::Quat_UnrotateVector(FQuat(CursorRotation), Location) + FVector(0, 0, 250)).GridSnap(XYSIZE);	// Unrotate the Hit Loc	
		FVector UnrotatedLoc = (UKismetMathLibrary::Quat_UnrotateVector(FQuat(CursorRotation), Location)).GridSnap(XYSIZE);	// Unrotate the Hit Loc	
		*NewLoc = UKismetMathLibrary::Quat_RotateVector(FQuat(CursorRotation), UnrotatedLoc + FVector(0, 0, 250));	// Assign the NewLoc vector with the Z adjust UnrotatedLoc
		Cursor->SetStaticMesh(WindowC);	// Set to correct mesh
				
		FCollisionShape CollisionShape;		// Next 5 lines for a sphere trace to check that our location is in a placed ship segment
		CollisionShape.ShapeType = ECollisionShape::Sphere;
		CollisionShape.SetSphere(XYSIZE);
		TArray<FHitResult> HitResults;
		GetWorld()->SweepMultiByChannel(HitResults, *NewLoc - 1, *NewLoc + 1, FQuat(), PLACEDHULL, CollisionShape);

		ValidPlace = false;
		FRotator RotOffset(0);
		FVector GSegExtent;
		FVector GSegLoc;
		*NewLoc = UKismetMathLibrary::Quat_RotateVector(FQuat(CursorRotation), UnrotatedLoc);
		if (HitResults.Num() == 1)	// If we are only in 1 segment
		{
			GainingSegment = Cast<AShipSegmentPlaced>(HitResults[0].Actor); // Cast the possible hit

			if (GainingSegment)	// If cast hit was success
			{
				GSegExtent = GainingSegment->DragData.Extent;
				GSegLoc = GainingSegment->DragData.Location;
				FVector Directional = ((GSegLoc - UnrotatedLoc).GetAbs()).GridSnap(XYSIZE);	// Find the mouse direction relative to segment center
				ValidPlace = (Directional.X == GSegExtent.X && Directional.Y <= GSegExtent.Y - 250); // If window is placed in hull X extent
				RotOffset = (ValidPlace) ? FRotator(0, 90, 0) : FRotator(0);	// If previous check X hull extent was true then update rotation offset
				ValidPlace = ValidPlace || (Directional.Y == GSegExtent.Y && Directional.X <= GSegExtent.X - 250);	// if window is place in hull Y extent
				
			}
		}
		if (DragStart != FVector(ALTDEFAULT))
		{
			FVector DSE = (GSegLoc - DragStart).GetAbs();
			ValidPlace &= (DSE.X == GSegExtent.X && DSE.Y <= GSegExtent.Y - 250) || (DSE.Y == GSegExtent.Y && DSE.X <= GSegExtent.X - 250);
			UpdateCursorDrag(*NewLoc, CursorRotation, RotOffset, ValidPlace);	// Update our cursor for item placement
		}
		else
			UpdateCursor(*NewLoc, CursorRotation, RotOffset, ValidPlace);	// Update our cursor for item placement
	}
	break;
	}
}

void AShipBuildTools::UpdateCursorDrag(FVector Loc, FRotator Rot, FRotator Local, bool Valid)
{
	int StencilValue = (Valid) ? 4 : 1;
	FLinearColor NewColor = (Valid) ? FLinearColor::Yellow : FLinearColor::Red;
	FVector DragDiff = (UKismetMathLibrary::Quat_UnrotateVector(FQuat(Rot), Loc).GridSnap(WORLDUNITSIZE) - DragStart.GridSnap(WORLDUNITSIZE)) * 0.5;
	FVector CenterDrag = UKismetMathLibrary::Quat_RotateVector(FQuat(Rot), DragStart + DragDiff + FVector(0,0,250));
	DragDiff = ((DragDiff.GetAbs()) / WORLDUNITSIZE) + 1;;
	FTransform CursorTForm(Rot, CenterDrag, FVector(fmax(DragDiff.GetMax(),1), 1,1));
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Scale is %f "), DragDiff.GetMax()));
	Cursor->SetWorldTransform(CursorTForm);
	Cursor->SetCustomDepthStencilValue(StencilValue);
	Material->SetVectorParameterValue("Color", NewColor);
	Cursor->AddLocalRotation(Local);
}

void AShipBuildTools::UpdateCursor(FVector Loc, FRotator Rot, FRotator Local, bool Valid)
{
	int StencilValue = (Valid) ? 4 : 1;
	FLinearColor NewColor = (Valid) ? FLinearColor::Yellow : FLinearColor::Red;
	Loc = UKismetMathLibrary::Quat_RotateVector(FQuat(Rot), (UKismetMathLibrary::Quat_UnrotateVector(FQuat(Rot), Loc)).GridSnap(XYSIZE) + FVector(0, 0, 250));
	FTransform CursorTForm(Rot, Loc, FVector(1));
	
	Cursor->SetWorldTransform(CursorTForm);
	Cursor->SetCustomDepthStencilValue(StencilValue);
	Material->SetVectorParameterValue("Color", NewColor);
	Cursor->AddLocalRotation(Local);
}

//Get the Location in world space
void AShipBuildTools::GetLocationRotation(FVector * Loc, FRotator * Rot)
{
	Loc->X = Location.X;
	Loc->Y = Location.Y;
	Loc->Z = Location.Z;
}

void AShipBuildTools::LeftRelease(FVector CurrentLoc)
{
	if (ValidPlace)
	{
		FVector Extent = (CurrentLoc - DragStart) * 0.5;
		FVector Center = DragStart + Extent + FVector(0, 0, 250);
		Extent = Extent.GetAbs();
		/*Extent += FVector(125,125,50);
		Extent.X = fmax(125, Extent.X);
		Extent.Y = fmax(125, Extent.Y);
		Extent.Z = fmax(175, Extent.Z);*/

		//FVector Center = Cursor->GetComponentLocation();
		//FVector Extent = (Cursor->GetComponentScale() * XYSIZE) + FVector(125);
		GainingSegment->AddItem(CurrentItemType, Center, Extent, nullptr);
	}
	ValidPlace = false;
	DragStart = FVector(ALTDEFAULT);
}

void AShipBuildTools::UpdateStartDrag(FVector StartLoc)	// Unrotated from manager
{
	DragStart = StartLoc;// +FVector(0, 0, 250);
}