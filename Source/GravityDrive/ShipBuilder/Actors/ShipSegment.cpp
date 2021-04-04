// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipSegment.h"

// Sets default values
AShipSegment::AShipSegment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Area = CreateDefaultSubobject<UBoxComponent>("Area");
	Area->SetBoxExtent(FVector(350.0, 350.0, 300.0));

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

