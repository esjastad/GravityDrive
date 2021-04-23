// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "../HISMManager.h"
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
		void SetDragData(ADragData * NewData, AHISMManager * NewHISMManager);
		void MouseOver(float ZLoc);
		bool GetBuildNext(APieceData * NextBuildPiece);
		AHISMManager * HISMManager;

		ADragData DragData;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* Area;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* InvalidArea;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh * Marker;

		UStaticMeshComponent * TEST;

		int CBIndex; //Current build index
};
