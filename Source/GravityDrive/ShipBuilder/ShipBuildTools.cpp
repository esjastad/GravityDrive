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
	DragRot = FRotator(ALTROT);
	GainingSegment = nullptr;
}

void AShipBuildTools::Update(FHitResult* NewHit, bool IsValid, BuildMode CurrentMode, AItemType CurrentItem)
{
	float TWL = 300;
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
			
			FVector OffSetter(0,0,250);
			Cursor->SetStaticMesh(WindowC);	// Set to correct mesh
			if (DragStart != FVector(ALTDEFAULT))
			{
				if (GainingSegment)	// If cast hit was success
				{
					ValidPlace = true;
					GSegExtent = GainingSegment->DragData.Extent;
					GSegLoc = GainingSegment->DragData.Location;
					FVector DSE = (DragStart - UnrotatedLoc);
					FVector Signage = DSE.GetSignVector();
					DSE.GetAbs();
					FVector MAXI = GSegLoc + (GSegExtent - FVector(TWL, TWL,0));
					FVector MINI = GSegLoc - (GSegExtent - FVector(TWL, TWL, 0));

					if (DragRot.Roll == 0)	// Wall Drag
					{
						if (DragRot.Yaw == 0) // X or Y Determine
						{							
							float XOff = fmax(fmin(MAXI.X, UnrotatedLoc.X), MINI.X);
							UnrotatedLoc = FVector(XOff, DragStart.Y, UnrotatedLoc.Z);
							//UnrotatedLoc = FVector(fmax(UnrotatedLoc.X, GSegLoc.X - ((GSegExtent.X - 300) * Signage.X)), DragStart.Y, UnrotatedLoc.Z);
						}
						else
						{
							float YOff = fmax(fmin(MAXI.Y, UnrotatedLoc.Y), MINI.Y);
							UnrotatedLoc = FVector(DragStart.X, YOff, UnrotatedLoc.Z);
						}
					}
					else	// Floor Ceil Drag
					{
						MAXI = GSegLoc + (GSegExtent - FVector(300, 300, 0));
						MINI = GSegLoc - (GSegExtent - FVector(300, 300, 0));

						float XOff = fmax(fmin(MAXI.X, UnrotatedLoc.X), MINI.X);
						float YOff = fmax(fmin(MAXI.Y, UnrotatedLoc.Y), MINI.Y);
						UnrotatedLoc = FVector(XOff, YOff, UnrotatedLoc.Z);
						OffSetter = FVector(0);
					}
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Local %f %f %f"), DragRot.Pitch, DragRot.Yaw, DragRot.Roll));
					UpdateCursorDrag(UnrotatedLoc + OffSetter, OffSetter, CursorRotation, DragRot, ValidPlace);	// Update our cursor for item placement
				}
			}
			else
			{
				if (HitResults.Num() == 1)	// If we are only in 1 segment
				{
					GainingSegment = Cast<AShipSegmentPlaced>(HitResults[0].Actor); // Cast the possible hit

					if (GainingSegment)	// If cast hit was success
					{
						ValidPlace = true;
						GSegExtent = GainingSegment->DragData.Extent;
						GSegLoc = GainingSegment->DragData.Location;

						FVector SignedDirectional = (GSegLoc - UnrotatedLoc).GridSnap(XYSIZE);	// Find the mouse direction relative to segment center
						FVector Directional = SignedDirectional.GetAbs();
						SignedDirectional = SignedDirectional.GetSignVector();
						if (Directional.Z == GSegExtent.Z)
						{
							if ((Directional.X >= GSegExtent.X - 250 || Directional.Y >= GSegExtent.Y - 250) && !CeilingPlaceCheck)
							{
								if (Directional.X - GSegExtent.X >= Directional.Y - GSegExtent.Y)
								{
									UnrotatedLoc = FVector(((Directional.X >= GSegExtent.X - TWL) ? GSegLoc.X - (GSegExtent.X * SignedDirectional.X) : UnrotatedLoc.X),
										GSegLoc.Y - ((fmin(Directional.Y, GSegExtent.Y - TWL)) * SignedDirectional.Y),
										UnrotatedLoc.Z);
									RotOffset = FRotator(0, 90, 0);
								}
								else
								{
									UnrotatedLoc = FVector(GSegLoc.X - ((fmin(Directional.X, GSegExtent.X - TWL)) * SignedDirectional.X),
										((Directional.Y >= GSegExtent.Y - TWL) ? GSegLoc.Y - (GSegExtent.Y * SignedDirectional.Y) : UnrotatedLoc.Y),
										UnrotatedLoc.Z);
									RotOffset = FRotator(0);
								}
							}
							else
							{
								UnrotatedLoc = FVector(GSegLoc.X - ((fmin(Directional.X, GSegExtent.X - 250)) * SignedDirectional.X),
									GSegLoc.Y - ((fmin(Directional.Y, GSegExtent.Y - 250)) * SignedDirectional.Y),
									UnrotatedLoc.Z);
								OffSetter = FVector(0);
								RotOffset = FRotator(0, 0, 90);
							}
						}
						else
						{
							if (Directional.X - GSegExtent.X >= Directional.Y - GSegExtent.Y)
							{
								UnrotatedLoc = FVector(GSegLoc.X - (GSegExtent.X * SignedDirectional.X),
									GSegLoc.Y - ((fmin(Directional.Y, GSegExtent.Y - TWL)) * SignedDirectional.Y),
									UnrotatedLoc.Z);
								RotOffset = FRotator(0, 90, 0);
							}
							else
							{
								UnrotatedLoc = FVector(GSegLoc.X - ((fmin(Directional.X, GSegExtent.X - TWL)) * SignedDirectional.X),
									GSegLoc.Y - (GSegExtent.Y * SignedDirectional.Y),
									UnrotatedLoc.Z);
								RotOffset = FRotator(0);
							}
						}
					}
				}
				DragRot = RotOffset.GridSnap(FRotator(90));
				UpdateCursor(UnrotatedLoc, OffSetter, CursorRotation, RotOffset, ValidPlace);	// Update our cursor for item placement
			}

			/*ValidPlace = (Directional.X == GSegExtent.X && Directional.Y <= GSegExtent.Y - 250); // If window is placed in hull X extent
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
								ValidPlace = (Directional.X <= GSegExtent.X - 250 && Directional.Y <= GSegExtent.Y - 250 && Directional.Z == GSegExtent.Z);

								if (ValidPlace)
								{
									OffSetter = FVector(0);
									RotOffset = FRotator(0, 0, 90);
								}
							}
						}*/

			/*if (DragStart != FVector(ALTDEFAULT))
			{
				FVector DSE = (GSegLoc - DragStart).GetAbs();
				if (RotOffset.Roll)
					ValidPlace &= (DSE.X <= GSegExtent.X - 250 && DSE.Y <= GSegExtent.Y - 250 && DSE.Z == GSegExtent.Z);
				else
					ValidPlace &= (DSE.X == GSegExtent.X && DSE.Y <= GSegExtent.Y - 250) || (DSE.Y == GSegExtent.Y && DSE.X <= GSegExtent.X - 250);			

				UpdateCursorDrag(UnrotatedLoc, OffSetter, CursorRotation, RotOffset, ValidPlace);	// Update our cursor for item placement
			}
			else*/
				
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
	
	FVector DragDiff = (Loc.GridSnap(XYSIZE) - DragStart.GridSnap(XYSIZE)) * 0.5;

	FVector CenterDrag = UKismetMathLibrary::Quat_RotateVector(FQuat(Rot), DragStart + DragDiff);

	DragDiff = ((DragDiff.GetAbs()) / WORLDUNITSIZE) + 1;;
	FVector DScale = (Local.Roll) ? FVector(DragDiff.X, 1, DragDiff.Y) : FVector(fmax(DragDiff.GetMax(), 1), 1, 1);
	//FVector DScale = FVector(1);//(Local.Roll) ? FVector(DragDiff.X, 1, DragDiff.Y) : FVector(fmax(DragDiff.GetMax(), 1), 1, 1);
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
	if (ValidPlace)
	{
		switch (CurrentItemType)
		{
		case AItemType::itemWindow:
		{
			FVector CScale = Cursor->GetComponentScale() * WORLDUNITSIZE;
			//FRotator CRot = Cursor->GetRelativeRotation();
			FRotator CRot = DragRot;

			FVector Extent = (CRot.Roll) ? FVector(CScale.X - 100, CScale.Z - 100, 1) : FVector((CRot.Yaw) ? 0 : CScale.X - 100, (CRot.Yaw) ? CScale.X - 100 : 0, CScale.Z);
			//FVector Center = Cursor->GetComponentLocation();
			FVector Center = UKismetMathLibrary::Quat_UnrotateVector(GetAttachParentActor()->GetActorQuat(), Cursor->GetComponentLocation());
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
	DragRot = FRotator(ALTROT);
}

void AShipBuildTools::UpdateStartDrag(FVector StartLoc)	// Unrotated from manager
{
	DragStart = UKismetMathLibrary::Quat_UnrotateVector(GetAttachParentActor()->GetActorQuat(), Cursor->GetComponentLocation());//StartLoc;// +FVector(0, 0, 250);
}