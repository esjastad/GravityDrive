// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipSegmentPlaced.h"

AShipSegmentPlaced::AShipSegmentPlaced() : AShipSegment()
{
	Area = CreateDefaultSubobject<UBoxComponent>("Area");
	Area->SetBoxExtent(FVector(350.0, 350.0, 300.0));
}

void AShipSegmentPlaced::Update(FVector Location, FVector Scale)
{
	SetActorLocation(Location + FVector(0, 0, 300));
	Area->SetBoxExtent(Scale + FVector(50, 50, 350));
}

void AShipSegmentPlaced::Finalize(bool Valid)
{
	if (Valid)
	{
		//Package up data and send to HISM, Add permanent segment actor
	}
	else
		Destroy();
}
