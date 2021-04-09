// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipManager.h"
#include "../Definitions.h"

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
	ValidDrag = false;
	LMB = false;
}

// Called when the game starts or when spawned
void AShipManager::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

// Called every frame
void AShipManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FHitResult CursorHit;
	if (PController->GetHitResultUnderCursor(BUILDPLANE, false, CursorHit)) {
		ValidDrag = ValidateDrag(&CursorHit);
		cBuildTools->Update(&CursorHit, ValidDrag);
		if (LMB)
		{
			FVector DragDelta = FVector(CursorHit.Location - DragStart).GridSnap(WORLDUNITSIZE) * 0.5;
			SegTemplate->Update(DragStart + DragDelta, DragDelta);
		}
	}
}

/** Validates the user drag based on ship level, check if inside bounds for ship building and checks for overlapping.
  * Returns a bool 
**/
bool AShipManager::ValidateDrag(FHitResult * CursorHit)
{
	FVector Location = CursorHit->Location.GridSnap(WORLDUNITSIZE).GetAbs();
	bool result = ((Location.X / WORLDUNITSIZE) < (ShipLevel * XYSIZE)) 
				&& ((CursorHit->Location.Y / WORLDUNITSIZE) < (ShipLevel * XYSIZE))
				&& ((CursorHit->Location.Z / ZFLOORSIZE) < (ShipLevel * ZSIZE));
	
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
	if (SegTemplate) { SegTemplate->Destroy(); }

}

// Handles left mouse click release
void AShipManager::HandleLeftRelease()
{
	if (ValidDrag)
	{
		//Have Ship Segment Add the Segment
	}

	LMB = false;
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
		SegTemplate->Initialize(SegmentType::cube);
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
	SpringArm->TargetArmLength = SpringArm->TargetArmLength - ZOOMSIZE;
}

// Controls Zooming out
void AShipManager::ZoomOut()
{
	SpringArm->TargetArmLength = SpringArm->TargetArmLength + ZOOMSIZE;
}

// Controls Moving Up a Z level
void AShipManager::UpZ()
{
	AddActorWorldOffset(FVector(0, 0, ZFLOORSIZE));
}

// Controls Moving Down a Z level
void AShipManager::DownZ()
{
	AddActorWorldOffset(FVector(0, 0, -ZFLOORSIZE));
}