// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "../HISMManager.h"
#include "VoxelWorld.h"
#include "VoxelAssets/VoxelDataAsset.h"
#include "ShipSegmentPlaced.generated.h"

/**
 * Placed Ship Segment, contains and handles placement data
 */
UCLASS()
class GRAVITYDRIVE_API AShipSegmentPlaced : public AShipSegment
{
	GENERATED_BODY()
	public:
		AShipSegmentPlaced();
		void SetDragData(ADragData * NewData, AHISMManager * NewHISMManager, AVoxelWorld * ShipRef);
		void MouseOver(int HitCount, float ZLevel);
		bool GetBuildNext(APieceData * NextBuildPiece);
		void UserDestroy();		// Called when user wants to destroy this actor from the ship, includes all palced sub items and notifies neighbors to redo shared walls

		AHISMManager * HISMManager;

		ADragData DragData;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* Area;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* InvalidArea;
		
		AVoxelWorld * VoxelShipRef;		// Reference to the voxel ship this segment belongs to

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UVoxelDataAsset * Cube;

		TArray<AShipSegmentPlaced*> TouchedSegments;
		TArray<AShipSegmentPlaced*> PathConnectedSegments;

		int CBIndex; //Current build index
};
