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
}

void AShipBuildTools::Update(FHitResult* NewHit, bool IsValid, BuildMode CurrentMode, AItemType CurrentItem)
{
	
	AActor * Parent = NewHit->Actor->GetAttachParentActor();	// Get Parent Reference
	FRotator CursorRotation(Parent->GetActorRotation());	// Get Parents rotation
	FVector * NewLoc(&NewHit->Location);
	
	//UpdateState(&NewHit->Location);
	Location = FVector(NewLoc->X, NewLoc->Y, NewLoc->Z);

	MPC->SetVectorParameterValue(FName("MouseLoc"), FLinearColor(*NewLoc));

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
		*NewLoc = (UKismetMathLibrary::Quat_RotateVector(FQuat(CursorRotation), UKismetMathLibrary::Quat_UnrotateVector(FQuat(CursorRotation), Location) + FVector(0,0,250))).GridSnap(1);
		Cursor->SetStaticMesh(WindowC);
		//CursorRotation = CursorRotation; Combine rotation of hit wall?
		
		FCollisionShape CollisionShape;
		CollisionShape.ShapeType = ECollisionShape::Sphere;
		CollisionShape.SetSphere(XYSIZE);
		TArray<FHitResult> HitResults;
		//FQuat QRot(GetAttachParentActor()->GetActorRotation());
		//FVector RLoc = UKismetMathLibrary::Quat_RotateVector(QRot, UKismetMathLibrary::Quat_UnrotateVector(QRot, CurrentLoc) + FVector(0, 0, 100));
		GetWorld()->SweepMultiByChannel(HitResults, Location - 1, Location + 1, FQuat(), PLACEDHULL, CollisionShape);
		if (HitResults.Num() == 1)
		{
			AShipSegmentPlaced * GainingSegment = Cast<AShipSegmentPlaced>(HitResults[0].Actor); // Cast the possible hit
			// 
			// Pass Hit Results to function in placed segment to carryout validity of placement based on TYPE ETC
			// Validity of placement when hovering better handled in tick... Maybe do a template that gets updated and handles its own validity....
			// Look into Build Tools mouse cursor and etc

			//GetWorld()->SweepSingleByChannel(HitResult, RLoc, RLoc + 1, FQuat(), PLACEDHULL, CollisionShape);
			//AShipSegmentPlaced * GainingSegment = Cast<AShipSegmentPlaced>(HitResults.Actor); // Cast the possible hit
			if (GainingSegment)
			{
				FVector Directional = ((GainingSegment->DragData.Location - UKismetMathLibrary::Quat_UnrotateVector(FQuat(CursorRotation), *NewLoc)).GetAbs()).GridSnap(1);
				if (Directional.X == GainingSegment->DragData.Extent.X && Directional.Y <= GainingSegment->DragData.Extent.Y - 200)
				{
					AActor * AttachedTo = GetAttachParentActor();
					FRotator Test = UKismetMathLibrary::MakeRotationFromAxes(AttachedTo->GetActorForwardVector(), AttachedTo->GetActorRightVector(), AttachedTo->GetActorUpVector());
					
					Cursor->SetWorldLocationAndRotation(*NewLoc, Test);	// Update Cursor World loc and Rotation
					Cursor->SetCustomDepthStencilValue(4);
					Material->SetVectorParameterValue("Color", FLinearColor::Yellow);
				}
				else if (Directional.Y == GainingSegment->DragData.Extent.Y && Directional.X <= GainingSegment->DragData.Extent.X - 200)
				{
					Cursor->SetWorldLocationAndRotation(*NewLoc, CursorRotation);	// Update Cursor World loc and Rotation
					Cursor->SetCustomDepthStencilValue(4);
					Material->SetVectorParameterValue("Color", FLinearColor::Yellow);
				}
				else
				{
					Cursor->SetWorldLocationAndRotation(*NewLoc, CursorRotation);	// Update Cursor World loc and Rotation
					Cursor->SetCustomDepthStencilValue(1);
					Material->SetVectorParameterValue("Color", FLinearColor::Red);
				}
				
			}
			else
			{
				Cursor->SetWorldLocationAndRotation(*NewLoc, CursorRotation);	// Update Cursor World loc and Rotation
				Cursor->SetCustomDepthStencilValue(1);
				Material->SetVectorParameterValue("Color", FLinearColor::Red);
			}
		}
		else
		{
			Cursor->SetWorldLocationAndRotation(*NewLoc, CursorRotation);	// Update Cursor World loc and Rotation
			Cursor->SetCustomDepthStencilValue(1);
			Material->SetVectorParameterValue("Color", FLinearColor::Red);
		}

		
	}
	break;
	}
}

//Get the Location in world space
void AShipBuildTools::GetLocationRotation(FVector * Loc, FRotator * Rot)
{
	Loc->X = Location.X;
	Loc->Y = Location.Y;
	Loc->Z = Location.Z;
}

