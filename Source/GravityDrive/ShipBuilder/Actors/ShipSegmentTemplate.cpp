// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipSegmentTemplate.h"

AShipSegmentTemplate::AShipSegmentTemplate() : AShipSegment()
{
	CurrentType = SegmentType::cube;
}

AShipSegmentTemplate::AShipSegmentTemplate(SegmentType type) : AShipSegment()
{
	CurrentType = type;
}

// Called when the game starts or when spawned
void AShipSegmentTemplate::BeginPlay()
{
	Super::BeginPlay();

}

void AShipSegmentTemplate::Update(FVector Location, FVector Scale)
{
	SetActorLocation(Location + FVector(0, 0, 300));
	switch (CurrentType) {
		case cube:

			break;
		case cylinder:

			break;
	}
}