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

void AShipBuildTools::UpdateState(FVector* NewLoc, FVector* NewNorm)
{
	Location = FVector(NewLoc->X, NewLoc->Y, NewLoc->Z);
	
	MPC->SetVectorParameterValue(FName("MouseLoc") , FLinearColor(*NewLoc));
	MPC->SetVectorParameterValue(FName("Normal"), FLinearColor(*NewNorm));
}

void AShipBuildTools::InitState()
{
	Location.X = 0;
	Location.Y = 0;
	Location.Z = 0;
}

void AShipBuildTools::Update(FHitResult* NewHit, bool IsValid)
{
	
	FRotator RotAmount = UKismetMathLibrary::MakeRotationFromAxes(GetActorForwardVector(), GetActorRightVector(), GetActorUpVector());
	FVector Rotated = UKismetMathLibrary::Quat_RotateVector(FQuat(RotAmount), NewHit->Location).GridSnap(WORLDUNITSIZE);
	FVector UnRotated = UKismetMathLibrary::Quat_UnrotateVector(FQuat(RotAmount), Rotated);
	FVector Snapped = UnRotated;// NewHit->Location.GridSnap(WORLDUNITSIZE);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Normal %f %f %f"), Rotated.X, Rotated.Y, Rotated.Z));
	//Material->SetVectorParameterValue(FName("Color"), (IsValid) ? FColor::Yellow : FColor::Red);
	Cursor->SetWorldLocationAndRotation(Snapped, FQuat(UKismetMathLibrary::MakeRotationFromAxes(NewHit->Normal, FVector(0), FVector(0))));
	//Cursor->SetWorldLocation(Snapped);
	UpdateState(&Snapped, &NewHit->Normal);
}

void AShipBuildTools::GetLocation(FVector * Loc)
{
	Loc->X = Location.X;
	Loc->Y = Location.Y;
	Loc->Z = Location.Z;
}

