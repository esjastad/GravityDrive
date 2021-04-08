// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ShipSegmentTemplate.h"
#include "Components/BoxComponent.h"
#include "ShipSegmentPlaced.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITYDRIVE_API AShipSegmentPlaced : public AShipSegment
{
	GENERATED_BODY()
	public:
		AShipSegmentPlaced();
		void Update(FVector Location, FVector Scale);
		void Finalize(bool Valid);

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* Area;

};
