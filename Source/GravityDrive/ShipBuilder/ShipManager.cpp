// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipManager.h"
#include "Math/UnrealMathUtility.h"
#include "VoxelTools/VoxelAssetTools.h"
#include "VoxelGenerators/VoxelGeneratorTools.h"
#include "VoxelTools/VoxelToolHelpers.h"
#include "VoxelData/VoxelDataIncludes.h"
#include "VoxelTools/VoxelDataTools.h"
#include "VoxelAssets/VoxelDataAssetData.inl"
#include "VoxelGenerators/VoxelGeneratorInstanceWrapper.h"
#include "../Definitions.h"
#include "DrawDebugHelpers.h" // Can remove for deployment builds

// Sets default values
AShipManager::AShipManager()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PController = nullptr;

	// Setup arm and camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>("Movement");
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");

	//Set Attachment Heirarchy
	RootComponent = Scene;
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	Mesh->SetupAttachment(RootComponent);

	//Set SpringArm Settings
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bDoCollisionTest = false;
	SpringArm->TargetArmLength = ARMDEFAULTLENGTH;

	ShipLevel = 1;
	UBSIndex = 0;
	ArmTarget = 1000;
	ValidDrag = false;
	LMB = false;
	SegType = SegmentType::cube;

}

// Called when the game starts or when spawned
void AShipManager::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	MPC = GetWorld()->GetParameterCollectionInstance(MPCAsset);
}

// Called every frame
void AShipManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FHitResult CursorHit;
	if (PController->GetHitResultUnderCursor(BUILDPLANE, false, CursorHit)) {
		// Adjust current mouse loc if it exceeds the max allowed build space relative to ship level
		FVector SignVec = CursorHit.Location.GetSignVector();
		FVector AbsVec = CursorHit.Location.GetAbs();
		float Maximum = (ShipLevel * WORLDUNITSIZE * XYSIZE);
		CursorHit.Location.X = (AbsVec.X > Maximum) ? Maximum * SignVec.X : CursorHit.Location.X;
		CursorHit.Location.Y = (AbsVec.Y > Maximum) ? Maximum * SignVec.Y : CursorHit.Location.Y;

		//Validate the drag is touching another and is not overlapping too far
		ValidDrag = ValidateDrag(&CursorHit);
		//Update the build tools
		cBuildTools->Update(&CursorHit, ValidDrag);
		if (LMB && CursorHit.Location.GridSnap(WORLDUNITSIZE) != DragStart.GridSnap(WORLDUNITSIZE))
		{
			FVector DragDelta = FVector(CursorHit.Location - DragStart).GridSnap(WORLDUNITSIZE) * 0.5;
			if (SegTemplate) SegTemplate->Update(DragStart + DragDelta, DragDelta, ValidDrag);
		}

		//Setup trace variables
		FVector Snapped = CursorHit.Location.GridSnap(WORLDUNITSIZE);
		TArray<FHitResult> HitResults;
		FCollisionShape CollisionShape;
		CollisionShape.ShapeType = ECollisionShape::Sphere;
		CollisionShape.SetSphere(25);

		//Trace for Ship Segment and call mouseover to display anchor points
		GetWorld()->SweepMultiByChannel(HitResults, Snapped + FVector(0, 0, WORLDUNITSIZE), Snapped, FQuat::FQuat(), ECC_GameTraceChannel2, CollisionShape);
		for (auto It = HitResults.CreateIterator(); It; It++)
		{
			AShipSegmentPlaced * SegOver = Cast<AShipSegmentPlaced>(It->Actor);
			SegOver->MouseOver(HitResults.Num(), Snapped.Z);
		}

	}
	UpdateArmLength(DeltaTime);
}

void AShipManager::UpdateSegType(SegmentType * newType)
{
	SegType = *newType;
}

void AShipManager::UpdateArmLength(float DTime)
{
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmTarget, DTime, 1);
	MPC->SetScalarParameterValue(FName("CamArmLength"), SpringArm->TargetArmLength);
}

/** Validates the user drag based on ship level, check if inside bounds for ship building and checks for overlapping.
  * Returns a bool 
**/
bool AShipManager::ValidateDrag(FHitResult * CursorHit)
{
	//Validate cursor is in the allowed bounds based on shiplevel
	FVector Location = CursorHit->Location.GridSnap(WORLDUNITSIZE).GetAbs();
	bool result = true; //(abs(Location.X / WORLDUNITSIZE) < (ShipLevel * XYSIZE)) 
				//&& (abs(CursorHit->Location.Y / WORLDUNITSIZE) < (ShipLevel * XYSIZE));
	// Do the next hit check for overlapping a segment and check the overlap is only the perimeter.
	// result &= true;
	if (LMB)
	{
		// Setup Box Trace
		TArray<FHitResult> HitResults;
		FCollisionShape CollisionShape;
		FVector HalfExtent = (0.5 * (DragStart - CursorHit->Location.GridSnap(WORLDUNITSIZE)));
		FVector Loc = DragStart - HalfExtent;
		Loc.Z += (HalfExtent.Z <= 0) ? ZFLOORHALF : 0;
		HalfExtent.Z += (HalfExtent.Z <= 0) ? -ZFLOORHALF : 0;
		HalfExtent = HalfExtent.GetAbs() + XYSIZE;
		CollisionShape.ShapeType = ECollisionShape::Box;
		CollisionShape.SetBox(HalfExtent);

		/*if (PlacedSegments.Num())
		{
			//This function returns true only if the hit was a block response, since these are overlap hits, the results array needs to be checked to determine if a hit was made.
			GetWorld()->SweepMultiByChannel(HitResults, Loc - 1, Loc + 1, FQuat::FQuat(), SHIPSEGMENT, CollisionShape);
			result &= (HitResults.Num()) ? true : false;
		}*/

		// If too far into another placed segment, bInvalid is True
		bool bInvalid = GetWorld()->SweepMultiByChannel(HitResults, Loc - 1, Loc + 1, FQuat::FQuat(), SHIPSEGINVALID, CollisionShape);
		
		result &= !bInvalid;
		
		//This draws a visualization of the trace to help with debugging
		//DrawDebugBox(GetWorld(), Loc, HalfExtent, FColor::Green, false, 1);
	}

	return result;
}

// Called to bind functionality to input
void AShipManager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind defined project inputs to respective functions
	InputComponent->BindAxis("MoveForward", this, &AShipManager::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AShipManager::MoveRight);
	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &AShipManager::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &AShipManager::ZoomOut);
	InputComponent->BindAction("UpZ", IE_Pressed, this, &AShipManager::UpZ);
	InputComponent->BindAction("DownZ", IE_Pressed, this, &AShipManager::DownZ);
	InputComponent->BindAction("LeftClick", IE_Pressed, this, &AShipManager::HandleLeftClick);
	InputComponent->BindAction("LeftClick", IE_Released, this, &AShipManager::HandleLeftRelease);
	InputComponent->BindAction("RightClick", IE_Pressed, this, &AShipManager::HandleRightClick);
}

void AShipManager::HandleRightClick()
{
	LMB = false;
	if (SegTemplate) { SegTemplate->Destroy(); }
}

// Handles left mouse click release
void AShipManager::HandleLeftRelease()
{
	if (LMB && ValidDrag)
	{
		//Have Ship Segment Add the Segment
		ADragData MyData;
		SegTemplate->PackageDrag(&MyData);

		LMB = false;
		SegTemplate->Destroy();

		NewPlacedSeg = GetWorld()->SpawnActor<AShipSegmentPlaced>(SegPlacedClass);
		NewPlacedSeg->SetDragData(&MyData, HISMManager);

		//Add to reference to the array of placed segments
		PlacedSegments.Add(NewPlacedSeg);
	}
	else
	{
		LMB = false;
		SegTemplate->Destroy();
	}
	
	//Temporary testing placement, this algorithm should be implemented elsewhere
	//test pointer to the placed segments array
	AShipSegmentPlaced ** tester = PlacedSegments.GetData();
	//Test stamp data for transform and stamp type
	APieceData stamp;
	//Voxel data object shape wrappers
	UVoxelTransformableGeneratorInstanceWrapper * CubeWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(Cube, VoxelShip->GetGeneratorInit());
	UVoxelTransformableGeneratorInstanceWrapper * WedgeWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(Wedge, VoxelShip->GetGeneratorInit());
	UVoxelTransformableGeneratorInstanceWrapper * CornerWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(Corner, VoxelShip->GetGeneratorInit());
	UVoxelTransformableGeneratorInstanceWrapper * InvertedWedgeWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(InvertedWedge, VoxelShip->GetGeneratorInit());
	UVoxelTransformableGeneratorInstanceWrapper * InvertedCornerWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(InvertedCorner, VoxelShip->GetGeneratorInit());
	UVoxelTransformableGeneratorInstanceWrapper * AngledWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(Angled, VoxelShip->GetGeneratorInit());
	UVoxelTransformableGeneratorInstanceWrapper * InnerCubeWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(CubeInner, VoxelShip->GetGeneratorInit());
	//Get the next segment to build from the child segment, change implementation for job requesting
	if (PlacedSegments.Num() > UBSIndex)
	{
		while (tester[UBSIndex]->GetBuildNext(&stamp))
		{
			//Get the value at the next location, if the value is not 1.0, then something is already there in the voxel graph
			float val = 0;
			UVoxelDataTools::GetValue(val, VoxelShip, VoxelShip->GlobalToLocal(stamp.Transform.GetLocation()));
			
			
			switch (stamp.StampType) {
			case AStampType::stampCube:
				(val > 0.0) ? UVoxelAssetTools::ImportAsset(VoxelShip, CubeWrapper, stamp.Transform, FVoxelIntBox()) : UVoxelAssetTools::ImportAsset(VoxelShip, InnerCubeWrapper, stamp.Transform, FVoxelIntBox());
				break;
			case AStampType::stampWedge:
				if (val > -0.25) { UVoxelAssetTools::ImportAsset(VoxelShip, WedgeWrapper, stamp.Transform, FVoxelIntBox()); }
				break;
			case AStampType::stampCorner:
				if (val > 0.9) { UVoxelAssetTools::ImportAsset(VoxelShip, CornerWrapper, stamp.Transform, FVoxelIntBox()); }
				break;
			case AStampType::stampInvertedWedge:
				UVoxelAssetTools::ImportAsset(VoxelShip, InvertedWedgeWrapper, stamp.Transform, FVoxelIntBox());
				break;
			case AStampType::stampInvertedCorner:
				UVoxelAssetTools::ImportAsset(VoxelShip, InvertedCornerWrapper, stamp.Transform, FVoxelIntBox());
				break;
			case AStampType::stampAngled:
				if (val > 0.9) { UVoxelAssetTools::ImportAsset(VoxelShip, AngledWrapper, stamp.Transform, FVoxelIntBox()); }
				break;
				
			}
			
		}
		++UBSIndex;
	}
}

// Called when possessed
void AShipManager::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// If possessed by player controller then spawn build tools.
	PController = Cast<APlayerController>(NewController);
	if (PController)
	{
		cBuildTools = GetWorld()->SpawnActor<AShipBuildTools>(cBuildToolClass);
		cBuildTools->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
		SetActorTickEnabled(true);
	}
}

// Called when unpossessed
void AShipManager::UnPossessed()
{
	// Destroy build tools if valid
	if (cBuildTools)
	{
		SetActorTickEnabled(false);
		PController = nullptr;
		cBuildTools->Destroy();
	}
}

// Handles the left mouse button click
void AShipManager::HandleLeftClick()
{
	if (Controller)
	{
		// Update DragStart
		cBuildTools->GetLocation(&DragStart);

		// Setup Segment Spawn Loc
		FVector SpawnLoc(0.0, 0.0, 250.0);
		SpawnLoc.AddBounded(DragStart);

		// Spawn Segment
		SegTemplate = GetWorld()->SpawnActor<AShipSegmentTemplate>(SegTemplateClass, FTransform(FRotator(0.0), SpawnLoc, FVector(1.0)), FActorSpawnParameters());
		SegTemplate->Initialize(SegType);
		LMB = true;
	}
	
}

// Controls forward and backward movement
void AShipManager::MoveForward(float Value)
{
	if ((Controller) && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

// Controls right and left movement
void AShipManager::MoveRight(float Value)
{
	if ((Controller) && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

// Controls Zooming in
void AShipManager::ZoomIn()
{
	ArmTarget = fmax(SpringArm->TargetArmLength * 0.5, 500);
	Movement->MaxSpeed = ArmTarget * 2;
	Movement->Acceleration = Movement->MaxSpeed * 3;
	Movement->Deceleration = Movement->Acceleration * 1.5;
}

// Controls Zooming out
void AShipManager::ZoomOut()
{
	ArmTarget = fmin(SpringArm->TargetArmLength *2, 64000);
	Movement->MaxSpeed = ArmTarget * 2;
	Movement->Acceleration = Movement->MaxSpeed * 3;
	Movement->Deceleration = Movement->Acceleration * 1.5;
}

// Controls Moving Up a Z level
void AShipManager::UpZ()
{
	SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, fmin((ShipLevel * (ZSIZE-1) * ZFLOORSIZE), GetActorLocation().Z) + ZFLOORSIZE));
	//AddActorWorldOffset(FVector(0, 0, ZFLOORSIZE));
	MPC->SetVectorParameterValue(FName("Position"), GetActorLocation() + FVector(0,0,ZFLOORHALF));
}

// Controls Moving Down a Z level
void AShipManager::DownZ()
{
	SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, fmax((ShipLevel * (ZSIZE-1) * -ZFLOORSIZE), GetActorLocation().Z) - ZFLOORSIZE));
	//AddActorWorldOffset(FVector(0, 0, -ZFLOORSIZE));
	MPC->SetVectorParameterValue(FName("Position"), GetActorLocation() + FVector(0, 0, ZFLOORHALF));
}