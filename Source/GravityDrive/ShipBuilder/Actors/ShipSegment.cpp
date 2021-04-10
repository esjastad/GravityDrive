// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipSegment.h"

// Sets default values
AShipSegment::AShipSegment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//Setup Root Component
	Scene = CreateDefaultSubobject<USceneComponent>("MyRoot");
	RootComponent = Scene;
}

// Called when the game starts or when spawned
void AShipSegment::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShipSegment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
