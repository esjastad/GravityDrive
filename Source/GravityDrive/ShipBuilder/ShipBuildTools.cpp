// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipBuildTools.h"

// Sets default values, is default constructor
AShipBuildTools::AShipBuildTools()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// Create components
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	Cursor = CreateDefaultSubobject<UStaticMeshComponent>("Cursor");
	// Set attach heirarchy
	RootComponent = Scene;
	Cursor->SetupAttachment(Scene);
	Cursor->SetWorldScale3D(FVector(1.01, 1.01, 1.01));

	//Set default state
	InitState();
}

// Called when the game starts or when spawned
void AShipBuildTools::BeginPlay()
{
	Super::BeginPlay();
	Material = UMaterialInstanceDynamic::Create(Cursor->GetMaterial(0), this);
	Cursor->SetMaterial(0, Material);

}

// Called every frame
void AShipBuildTools::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShipBuildTools::UpdateState(FVector* NewLoc)
{
	
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Location is %s"), *NewLoc->ToString()));
	Location = FVector(NewLoc->X, NewLoc->Y, NewLoc->Z);
	Cursor->SetWorldLocation(Location);
}

void AShipBuildTools::InitState()
{
	Location.X = 0;
	Location.Y = 0;
	Location.Z = 0;
}

void AShipBuildTools::Update(FHitResult* NewHit)
{
	FVector Snapped = NewHit->Location.GridSnap(100.0);
	UpdateState(&Snapped);
}

void AShipBuildTools::GetLocation(FVector * Loc)
{
	Loc->X = Location.X;
	Loc->Y = Location.Y;
	Loc->Z = Location.Z;
}

void AShipBuildTools::ValidDrag()
{
	Material->SetVectorParameterValue(FName("Color"), FColor::Yellow);
}

void AShipBuildTools::InvalidDrag()
{
	Material->SetVectorParameterValue(FName("Color"), FColor::Red);
}