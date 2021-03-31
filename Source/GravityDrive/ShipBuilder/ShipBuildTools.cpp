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

