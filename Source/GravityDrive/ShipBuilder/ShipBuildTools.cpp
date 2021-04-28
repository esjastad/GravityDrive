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

void AShipBuildTools::UpdateState(FVector* NewLoc)
{
	Location = FVector(NewLoc->X, NewLoc->Y, NewLoc->Z);
	
	MPC->SetVectorParameterValue(FName("MouseLoc") , FLinearColor(*NewLoc));
}

void AShipBuildTools::InitState()
{
	Location.X = 0;
	Location.Y = 0;
	Location.Z = 0;
}

void AShipBuildTools::Update(FHitResult* NewHit, bool IsValid)
{
	//Get the Rotation amount from a 0 rotation
	//FRotator RotAmount = UKismetMathLibrary::MakeRotationFromAxes(GetActorForwardVector(), GetActorRightVector(), GetActorUpVector());
	//Unrotate the current Hit Location and snap it to the grid
	//FVector UnRotated = UKismetMathLibrary::Quat_UnrotateVector(FQuat(RotAmount), NewHit->Location).GridSnap(WORLDUNITSIZE);
	//Rerotate the snapped hit to line up with the world rotation
	//FVector Rotated = UKismetMathLibrary::Quat_RotateVector(FQuat(RotAmount), UnRotated);
	//Redundant vector needs to be removed
	//FVector Snapped = Rotated;
	
	Cursor->SetWorldLocationAndRotation(NewHit->Location, FQuat(UKismetMathLibrary::MakeRotationFromAxes(NewHit->Normal, FVector(0), FVector(0))));
	
	UpdateState(&NewHit->Location);
}

//Get the Location in world space
void AShipBuildTools::GetLocationRotation(FVector * Loc, FRotator * Rot)
{
	Loc->X = Location.X;
	Loc->Y = Location.Y;
	Loc->Z = Location.Z;
}

