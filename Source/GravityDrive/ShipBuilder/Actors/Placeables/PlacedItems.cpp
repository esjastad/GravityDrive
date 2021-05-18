// ©, 2021, Erik Jastad. All Rights Reserved


#include "PlacedItems.h"

// Sets default values
APlacedItems::APlacedItems()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Area = CreateDefaultSubobject<UBoxComponent>("Area");
	RootComponent = Area;
}

// Called when the game starts or when spawned
void APlacedItems::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

// Called every frame
void APlacedItems::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

