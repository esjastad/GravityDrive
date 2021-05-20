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
		bool CeilingPlaceCheck = false;
		FVector UnrotatedLoc = (UKismetMathLibrary::Quat_UnrotateVector(FQuat(CursorRotation), Location)).GridSnap(XYSIZE);	// Unrotate the Hit Loc	
		*NewLoc = UKismetMathLibrary::Quat_RotateVector(FQuat(CursorRotation), UnrotatedLoc + FVector(0, 0, 250));	// Assign the NewLoc vector with the Z adjust UnrotatedLoc
		FVector SubLoc = UKismetMathLibrary::Quat_RotateVector(FQuat(CursorRotation), UnrotatedLoc);

		FCollisionShape CollisionShape;		// Next 5 lines for a sphere trace to check that our location is in a placed ship segment
		CollisionShape.ShapeType = ECollisionShape::Sphere;
		CollisionShape.SetSphere(XYSIZE);
		
		HitResults.Empty();
		GetWorld()->SweepMultiByChannel(HitResults, *NewLoc - 1, *NewLoc + 1, FQuat(), PLACEDHULL, CollisionShape);

		if (!HitResults.Num())
		{
			GetWorld()->SweepMultiByChannel(HitResults, SubLoc - 1, SubLoc + 1, FQuat(), PLACEDHULL, CollisionShape);
			CeilingPlaceCheck = true;
		}

		ValidPlace = false;
		FRotator RotOffset(0);
		FVector GSegExtent;
		FVector GSegLoc;
		

		switch (CurrentItem)
		{
		case AItemType::itemWindow:
		{	
			//*NewLoc = UKismetMathLibrary::Quat_RotateVector(FQuat(CursorRotation), UnrotatedLoc);
			//*NewLoc = UnrotatedLoc; 
			FVector OffSetter(0,0,250);
			Cursor->SetStaticMesh(WindowC);	// Set to correct mesh
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

					ValidPlace &= !CeilingPlaceCheck;
					if (!ValidPlace)
					{
						TArray<FHitResult> SubCheck;	// Check to make sure bottom Z is only 1 hit
						
						GetWorld()->SweepMultiByChannel(SubCheck, SubLoc - 1, SubLoc + 1, FQuat(), PLACEDHULL, CollisionShape); 
						if (SubCheck.Num() < 2)
						{
							// Check if a Z placement is legit
							ValidPlace = (Directional.X <= GSegExtent.X - 250 && Directional.Y <= GSegExtent.Y - 250);

							if (ValidPlace)
							{
								OffSetter = FVector(0);
								RotOffset = FRotator(0, 0, 90);
							}
						}
					}
				}
			}
			if (DragStart != FVector(ALTDEFAULT))
			{
				FVector DSE = (GSegLoc - DragStart).GetAbs();
				if (RotOffset.Roll)
					ValidPlace &= (DSE.X <= GSegExtent.X - 250 && DSE.Y <= GSegExtent.Y - 250);
				else
					ValidPlace &= (DSE.X == GSegExtent.X && DSE.Y <= GSegExtent.Y - 250) || (DSE.Y == GSegExtent.Y && DSE.X <= GSegExtent.X - 250);			

				UpdateCursorDrag(UnrotatedLoc, OffSetter, CursorRotation, RotOffset, ValidPlace);	// Update our cursor for item placement
			}
			else
				UpdateCursor(UnrotatedLoc, OffSetter, CursorRotation, RotOffset, ValidPlace);	// Update our cursor for item placement
		}
		break;
		case AItemType::itemDoor:
		{
			//*NewLoc = UKismetMathLibrary::Quat_RotateVector(FQuat(CursorRotation), UnrotatedLoc + FVector(0,0, -100));
			//*NewLoc = UnrotatedLoc + FVector(0, 0, 200);
			Cursor->SetStaticMesh(DoorC);	// Set to correct mesh
			if (HitResults.Num() == 2)	// If we are hitting 2 segments
			{
				GainingSegment = Cast<AShipSegmentPlaced>(HitResults[0].Actor); // Cast the possible hit

				if (GainingSegment)	// If cast hit was success
				{
					GSegExtent = GainingSegment->DragData.Extent;
					GSegLoc = GainingSegment->DragData.Location;
					FVector Directional = ((GSegLoc - UnrotatedLoc).GetAbs()).GridSnap(XYSIZE);	// Find the mouse direction relative to segment center
					ValidPlace = (Directional.X == GSegExtent.X && Directional.Y <= GSegExtent.Y - 250); // If window is placed in hull X extent
					RotOffset = (ValidPlace) ? FRotator(0, 0, 0) : FRotator(0, 90, 0);	// If previous check X hull extent was true then update rotation offset
					ValidPlace = ValidPlace || (Directional.Y == GSegExtent.Y && Directional.X <= GSegExtent.X - 250);	// if window is place in hull Y extent

				}
			}
			
			UpdateCursor(UnrotatedLoc, FVector(0,0,175),CursorRotation, RotOffset, ValidPlace);	// Update our cursor for item placement
		}
		break;
		case AItemType::itemStairs:
		{

		}
		break;
		}
		
	}
	break;
	}
}

void AShipBuildTools::UpdateCursorDrag(FVector Loc, FVector Offset, FRotator Rot, FRotator Local, bool Valid)
{
	int StencilValue = (Valid) ? 4 : 1;
	FLinearColor NewColor = (Valid) ? FLinearColor::Yellow : FLinearColor::Red;
	
	FVector DragDiff = (Loc.GridSnap(WORLDUNITSIZE) - DragStart.GridSnap(WORLDUNITSIZE)) * 0.5;
	FVector CenterDrag = UKismetMathLibrary::Quat_RotateVector(FQuat(Rot), DragStart + DragDiff + Offset);
	DragDiff = ((DragDiff.GetAbs()) / WORLDUNITSIZE) + 1;;
	FVector DScale = (Local.Roll) ? FVector(DragDiff.X, 1, DragDiff.Y) : FVector(fmax(DragDiff.GetMax(), 1), 1, 1);
	FTransform CursorTForm(Rot, CenterDrag, DScale);
	
	Cursor->SetWorldTransform(CursorTForm);
	Cursor->SetCustomDepthStencilValue(StencilValue);
	Material->SetVectorParameterValue("Color", NewColor);
	Cursor->AddLocalRotation(Local);
}

void AShipBuildTools::UpdateCursor(FVector Loc, FVector Offset, FRotator Rot, FRotator Local, bool Valid)
{
	int StencilValue = (Valid) ? 4 : 1;
	FLinearColor NewColor = (Valid) ? FLinearColor::Yellow : FLinearColor::Red;
	Loc = UKismetMathLibrary::Quat_RotateVector(FQuat(Rot), Loc.GridSnap(XYSIZE) + Offset);
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
	//if (ValidPlace)
	{
		switch (CurrentItemType)
		{
		case AItemType::itemWindow:
		{
			FVector Extent = (CurrentLoc - DragStart) * 0.5;
			FVector Addition = (Extent.X != 0 && Extent.Y != 0) ? FVector(0) : FVector(0, 0, 250);
			FVector Center = DragStart + Extent + Addition;
			Extent = Extent.GetAbs();

			GainingSegment->AddItem(CurrentItemType, Center, Extent, nullptr);
		}
		break;
		case AItemType::itemDoor:
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));
			GainingSegment->AddItem(CurrentItemType, CurrentLoc, FVector(0), &HitResults);
		}
		break;
		case AItemType::itemStairs:
		{

		}
		break;
		}
	}

	ValidPlace = false;
	DragStart = FVector(ALTDEFAULT);
}

void AShipBuildTools::UpdateStartDrag(FVector StartLoc)	// Unrotated from manager
{
	DragStart = StartLoc;// +FVector(0, 0, 250);
}