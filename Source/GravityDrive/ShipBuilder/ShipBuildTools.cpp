// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipBuildTools.h"

// Sets default values, is default constructor
AShipBuildTools::AShipBuildTools()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create components
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	Cursor = CreateDefaultSubobject<UStaticMeshComponent>("Cursor");
	// Set attach heirarchy
	RootComponent = Scene;
	Cursor->SetupAttachment(Scene);

	//Set default state
	InitState();
}

// Called when the game starts or when spawned
void AShipBuildTools::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShipBuildTools::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShipBuildTools::UpdateState(FVector* NewLoc)
{
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Location is %s"), *NewLoc->ToString()));
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
	float x = roundf(NewHit->Location.X / 100.0) * 100.0;
	float y = roundf(NewHit->Location.Y / 100.0) * 100.0;
	float z = roundf(NewHit->Location.Z / 100.0) * 100.0;

	FVector Snapped = FVector(x,y,0.0);
	UpdateState(&Snapped);
}