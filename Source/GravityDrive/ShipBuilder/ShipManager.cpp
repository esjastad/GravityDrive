// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipManager.h"
#include "Kismet/KismetMathLibrary.h"
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
	CurrentLoc = FVector(0);
	bMMB = false;
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

		//Get the Rotation amount from a 0 rotation
		FRotator RotAmount = GetAttachParentActor()->GetActorRotation();
		//Unrotate the current Hit Location and snap it to the grid
		FVector UnRotated = UKismetMathLibrary::Quat_UnrotateVector(FQuat(RotAmount), CursorHit.Location).GridSnap(WORLDUNITSIZE);
		
		// Adjust the Unrotated mouse loc, if it exceeds the max allowed build space relative to ship level then change the value to max.
		FVector SignVec = UnRotated.GetSignVector();
		FVector AbsVec = UnRotated.GetAbs();
		float Maximum = (ShipLevel * WORLDUNITSIZE * XYSIZE);
		//Create New Hit Loc Vector that is within adjusted bounds if needed
		FVector Adjusted = FVector(((AbsVec.X > Maximum) ? Maximum * SignVec.X : UnRotated.X), ((AbsVec.Y > Maximum) ? Maximum * SignVec.Y : UnRotated.Y), UnRotated.Z).GridSnap(WORLDUNITSIZE);
		//Rotate the adjust hit to line back up with the current world rotation
		FVector CSnapped = UKismetMathLibrary::Quat_RotateVector(FQuat(RotAmount), Adjusted);
		//Update CurrentData
		CursorHit.Location = CSnapped;
		
		CurrentRotation = RotAmount;

		
		//Update the build tools
		cBuildTools->Update(&CursorHit, ValidDrag);
		//Update the current drag template if it exists
		if (CursorHit.Location != CurrentLoc)
		{
			CurrentLoc = CSnapped;
			if (LMB)
			{
				FVector Start = FVector(UKismetMathLibrary::Quat_UnrotateVector(FQuat(CurrentRotation), DragStart)).GridSnap(WORLDUNITSIZE);
				FVector HalfExtent = (Adjusted - Start).GridSnap(WORLDUNITSIZE) * 0.5;
				FVector DragDelta = UKismetMathLibrary::Quat_RotateVector(FQuat(RotAmount), HalfExtent);
				FVector Center = Start + HalfExtent;
				Center = UKismetMathLibrary::Quat_RotateVector(FQuat(RotAmount), Center);

				//Validate the drag is touching another and is not overlapping too far
				ValidDrag = ValidateDrag(HalfExtent, Start);
				//Update the Drag Template if valid
				if (SegTemplate) SegTemplate->Update(Center, DragDelta, ValidDrag, CurrentRotation);
			}		
		}

		//Setup trace variables
		FVector RotOffsetSnapped = UKismetMathLibrary::Quat_RotateVector(FQuat(RotAmount), FVector(0, 0, WORLDUNITSIZE));
		TArray<FHitResult> HitResults;
		FCollisionShape CollisionShape;
		CollisionShape.ShapeType = ECollisionShape::Sphere;
		CollisionShape.SetSphere(25);

		//Trace for Ship Segment and call mouseover to display anchor points
		GetWorld()->SweepMultiByChannel(HitResults, CSnapped + RotOffsetSnapped, CSnapped, FQuat::FQuat(), ECC_GameTraceChannel2, CollisionShape);
		for (auto It = HitResults.CreateIterator(); It; It++)
		{
			AShipSegmentPlaced * SegOver = Cast<AShipSegmentPlaced>(It->Actor);
			SegOver->MouseOver(HitResults.Num(), UnRotated.Z);
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

/** Takes in the HalfExtent Vector of the Current Drag and the Start Location of the Drag
  * Traces the Box Drag to check for valid overlapping
  * Returns a bool (True if the drag is valid, False if (not touching  && not first piece) || overlapping too far)
**/
bool AShipManager::ValidateDrag(FVector HalfExtent, FVector Start)
{
	//Value returned at end of function
	bool result = true; 
				
	// Setup Trace variables
	TArray<FHitResult> HitResults;
	FCollisionShape CollisionShape;
	FVector CenterLoc = Start + HalfExtent;
	//Adjust the Center and Half Extent if drag is positive in Z
	CenterLoc.Z += (HalfExtent.Z >= 0) ? ZFLOORHALF : 0;
	HalfExtent.Z += (HalfExtent.Z >= 0) ? ZFLOORHALF : 0;
	HalfExtent = HalfExtent.GetAbs() + XYSIZE;
	CollisionShape.ShapeType = ECollisionShape::Box;
	CollisionShape.SetBox(HalfExtent);
	//Rotate the center vector to align with the current world rotation
	CenterLoc = UKismetMathLibrary::Quat_RotateVector(FQuat(CurrentRotation), CenterLoc);

	//Used to ensure we are touching another ship piece
	/*if (PlacedSegments.Num())
	{
		//This function returns true only if the hit was a block response, since these are overlap hits, the results array needs to be checked to determine if a hit was made.
		GetWorld()->SweepMultiByChannel(HitResults, CenterLoc - 1, CenterLoc + 1, FQuat(CurrentRotation), SHIPSEGMENT, CollisionShape);
		result &= (HitResults.Num()) ? true : false;
	}*/

	// If too far into another placed segment, bInvalid is True so invert and AND the result
	result &= !(GetWorld()->SweepMultiByChannel(HitResults, CenterLoc - 1, CenterLoc + 1, FQuat(CurrentRotation), SHIPSEGINVALID, CollisionShape));
		
	//This draws a visualization of the trace to help with debugging
	//DrawDebugBox(GetWorld(), CenterLoc, HalfExtent, FColor::Green, false, 1);
	
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
	InputComponent->BindAxis("LookRight", this, &AShipManager::TurnRight);
	InputComponent->BindAxis("LookUp", this, &AShipManager::LookUp);
	InputComponent->BindAction("MiddleMouse", IE_Pressed, this, &AShipManager::UpdateCanRotate);
	InputComponent->BindAction("MiddleMouse", IE_Released, this, &AShipManager::UpdateCanRotate);
}

// When player presses/releases middle mouse button, invert state variable
void AShipManager::UpdateCanRotate()
{
	bMMB = !bMMB;
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
		

		NewPlacedSeg = GetWorld()->SpawnActor<AShipSegmentPlaced>(SegPlacedClass);
		NewPlacedSeg->SetDragData(&MyData, HISMManager);
		NewPlacedSeg->AttachToActor(GetAttachParentActor(), FAttachmentTransformRules::KeepWorldTransform);
		//Add to reference to the array of placed segments
		PlacedSegments.Add(NewPlacedSeg);
		SegTemplate->Destroy();
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
			
			FTransform Proper = stamp.Transform;

			//Get Rotations and Rotate the location accordingly
			FRotator PROT = GetAttachParentActor()->GetActorRotation();
			FRotator TROT = FRotator(Proper.GetRotation());		
			FVector Rotated = UKismetMathLibrary::Quat_RotateVector(FQuat(PROT), Proper.GetLocation());

			
			
			//Add offset to location... Need to fix the ZFLOORHALF and check boolean for adding....
			FVector TANGO = UKismetMathLibrary::Quat_UnrotateVector(FQuat(tester[UBSIndex]->DragData.Rotation), tester[UBSIndex]->DragData.Location);
			TANGO += (tester[UBSIndex]->DragData.OffsetAdded) ? FVector(0, 0, -ZFLOORHALF) : FVector(0, 0, 0);

			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("is offset %d"), int(tester[UBSIndex]->DragData.OffsetAdded)));
			Rotated = UKismetMathLibrary::Quat_RotateVector(FQuat(PROT), (TANGO + Proper.GetLocation() ));
			//Rotated += (tester[UBSIndex]->DragData.OffsetAdded) ? FVector(0, 0, -ZFLOORHALF) : FVector(0, 0, ZFLOORHALF);
			//+((tester[UBSIndex]->DragData.OffsetAdded) ? FVector(0, 0, -ZFLOORHALF) : FVector(0, 0, ZFLOORHALF))
			Proper.SetLocation(Rotated);

			//Find Rotator to use by rotating these axis and making a rotator from them
			FVector UpVec = UKismetMathLibrary::Quat_RotateVector(FQuat(TROT), FVector(0, 0, 1)).GridSnap(1);
			UpVec = UKismetMathLibrary::Quat_RotateVector(FQuat(PROT), UpVec);
			FVector Right = UKismetMathLibrary::Quat_RotateVector(FQuat(TROT), FVector(0, 1, 0)).GridSnap(1);
			Right = UKismetMathLibrary::Quat_RotateVector(FQuat(PROT), Right);
			FVector Front = UKismetMathLibrary::Quat_RotateVector(FQuat(TROT), FVector(1, 0, 0)).GridSnap(1);
			Front = UKismetMathLibrary::Quat_RotateVector(FQuat(PROT), Front);

			//Set Rotator 
			FRotator WHAT = UKismetMathLibrary::MakeRotationFromAxes(Front, Right, UpVec);
			Proper.SetRotation(FQuat(WHAT));
			

			//Find location value to determine if voxels are already placed....
			UVoxelDataTools::GetValue(val, VoxelShip, VoxelShip->GlobalToLocal(Rotated));

			switch (stamp.StampType) {
			case AStampType::stampCube:
				(val > 0.0) ? UVoxelAssetTools::ImportAsset(VoxelShip, CubeWrapper, Proper, FVoxelIntBox()) : UVoxelAssetTools::ImportAsset(VoxelShip, InnerCubeWrapper, Proper, FVoxelIntBox());
				break;
			case AStampType::stampWedge:
				if (val > -0.25) { UVoxelAssetTools::ImportAsset(VoxelShip, WedgeWrapper, Proper, FVoxelIntBox()); }
				break;
			case AStampType::stampCorner:
				if (val > 0.9) { UVoxelAssetTools::ImportAsset(VoxelShip, CornerWrapper, Proper, FVoxelIntBox()); }
				break;
			case AStampType::stampInvertedWedge:
				UVoxelAssetTools::ImportAsset(VoxelShip, InvertedWedgeWrapper, Proper, FVoxelIntBox());
				break;
			case AStampType::stampInvertedCorner:
				UVoxelAssetTools::ImportAsset(VoxelShip, InvertedCornerWrapper, Proper, FVoxelIntBox());
				break;
			case AStampType::stampAngled:
				if (val > 0.9) { UVoxelAssetTools::ImportAsset(VoxelShip, AngledWrapper, Proper, FVoxelIntBox()); }
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
		cBuildTools->AttachToActor(GetAttachParentActor(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		FVector relativeLoc = GetTransform().InverseTransformPosition(GetAttachParentActor()->GetActorLocation());
		cBuildTools->AddActorLocalOffset(FVector(0, 0, -relativeLoc.Z));
		SetActorTickEnabled(true);
	}
}

// Called when unpossessed
void AShipManager::UnPossessed()
{
	Super::UnPossessed();
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
		DragStart = CurrentLoc;
		DragRotation = CurrentRotation;
		// Setup Segment Spawn Loc
		FVector SpawnLoc(0.0, 0.0, 250.0);
		SpawnLoc.AddBounded(DragStart);

		// Spawn Segment
		SegTemplate = GetWorld()->SpawnActor<AShipSegmentTemplate>(SegTemplateClass, FTransform(FRotator(0.0), SpawnLoc, FVector(1.0)), FActorSpawnParameters());
		SegTemplate->Initialize(SegType);
		SegTemplate->AttachToActor(GetAttachParentActor(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		LMB = true;
	}
	
}

void AShipManager::LookUp(float Value)
{
	if (bMMB)
		SpringArm->AddRelativeRotation((abs(SpringArm->GetRelativeRotation().Pitch + Value) >= 90.0) ? FRotator(0, 0, 0) : FRotator(Value, 0, 0));
}

void AShipManager::TurnRight(float Value)
{
	if (bMMB)
		AddActorLocalRotation(FRotator(0, Value, 0));
}

// Controls forward and backward movement
void AShipManager::MoveForward(float Value)
{
	if ((Controller) && Value != 0.0f)
	{
		/*const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);*/
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

// Controls right and left movement
void AShipManager::MoveRight(float Value)
{
	if ((Controller) && Value != 0.0f)
	{
		/*const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);*/
		AddMovementInput(GetActorRightVector(), Value);
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
	//Check that added value does not exceed Ship level bounds...
	FVector Unrot = -GetTransform().InverseTransformPosition(GetAttachParentActor()->GetActorLocation()).GridSnap(WORLDUNITSIZE);
	Unrot.Z += ZFLOORSIZE;
	if (Unrot.Z < (ShipLevel * (ZSIZE - 1) * ZFLOORSIZE))
	{
		AddActorLocalOffset(FVector(0, 0, ZFLOORSIZE));
		//Update Build Tools
		cBuildTools->AddActorLocalOffset(FVector(0, 0, ZFLOORSIZE));
	}
	
	//SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, fmin((ShipLevel * (ZSIZE-1) * ZFLOORSIZE), GetActorLocation().Z) + ZFLOORSIZE));
	
	//Set Position for MPC
	Unrot = UKismetMathLibrary::Quat_RotateVector(FQuat(CurrentRotation), Unrot + FVector(0, 0, ZFLOORHALF));
	MPC->SetVectorParameterValue(FName("Position"), Unrot);
}

// Controls Moving Down a Z level
void AShipManager::DownZ()
{
	FVector Unrot = -GetTransform().InverseTransformPosition(GetAttachParentActor()->GetActorLocation()).GridSnap(WORLDUNITSIZE);
	Unrot.Z += -ZFLOORSIZE;
	if (Unrot.Z > (ShipLevel * (ZSIZE - 1) * -ZFLOORSIZE))
	{
		AddActorLocalOffset(FVector(0, 0, -ZFLOORSIZE));
		//Update Build Tools
		cBuildTools->AddActorLocalOffset(FVector(0, 0, -ZFLOORSIZE));
	}

	//SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, fmax((ShipLevel * (ZSIZE-1) * -ZFLOORSIZE), GetActorLocation().Z) - ZFLOORSIZE));
	
	//Set Position for MPC
	Unrot = UKismetMathLibrary::Quat_RotateVector(FQuat(CurrentRotation), Unrot + FVector(0, 0, ZFLOORHALF));
	MPC->SetVectorParameterValue(FName("Position"), Unrot );
}