// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipManager.h"
#define XYSIZE 50.0
#define ZSIZE 5.0
#define WORLDUNITSIZE 100.0
#define ZFLOORSIZE 500.0
#define ZOOMSIZE 300.0
#define ARMDEFAULTLENGTH 1000.0
#define BUILDPLANE ECC_GameTraceChannel1

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

		if (ValidDrag)
			cBuildTools->ValidDrag();
		else
			cBuildTools->InvalidDrag();

		cBuildTools->Update(&CursorHit);
	}
}

/** Validates the user drag based on ship level, check if inside bounds for ship building and checks for overlapping.
  * Returns a bool 
**/
bool AShipManager::ValidateDrag(FHitResult * CursorHit)
{
	FVector Location = CursorHit->Location.GridSnap(100.0).GetAbs();
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
}

// Handles left mouse click release
void AShipManager::HandleLeftRelease()
{

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
{//GetActorLocation().AddBounded(FVector(0.0, 0.0, 300.0))
	if (Controller)
	{
		// Update DragStart
		cBuildTools->GetLocation(&DragStart);

		// Setup Segment Spawn Loc
		FVector SpawnLoc(0.0, 0.0, 250.0);
		SpawnLoc.AddBounded(DragStart);

		// Spawn Segment
		SelectedSegment = GetWorld()->SpawnActor<AShipSegment>(AShipSegment::StaticClass(), FTransform(FRotator(0.0), SpawnLoc, FVector(1.0)),FActorSpawnParameters());
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