// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "../HISMManager.h"
#include "ShipSegment.h"
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
		void AddTouchingSegment(AShipSegmentPlaced * SegToAdd);
		void RemoveTouchingSegment(AShipSegmentPlaced * SegToRemove);
		void AddStampToVoxelShip(AStampType Stamp, FTransform TForm);
		void TransformLocToWorld(FTransform * TForm);
		void AddItem(AItemType type, FVector CenterLoc, FVector Extent, TArray<FHitResult> * hitresults);

		AHISMManager * HISMManager;

		ADragData DragData;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* Area;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* InvalidArea;
		
		AVoxelWorld * VoxelShipRef;		// Reference to the voxel ship this segment belongs to

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UVoxelDataAsset * Cube;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UVoxelDataAsset * CubeInner;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UVoxelDataAsset * Wedge;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UVoxelDataAsset * Corner;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UVoxelDataAsset * InvertedWedge;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UVoxelDataAsset * InvertedCorner;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UVoxelDataAsset * Angled;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UVoxelDataAsset * CubeAngled;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UVoxelDataAsset * BigFrame;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UVoxelDataAsset * BigFrameInner;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UVoxelDataAsset * CubeHalfFrame;

		// Touched segments array, and connected
		TArray<AShipSegmentPlaced*> TouchedSegments;
		TArray<AShipSegmentPlaced*> PathConnectedSegments;

		int CBIndex; //Current build index
};
