// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"

//These defines should be seperated out into their own files where needed
#define XYSIZE 50.0
#define ZSIZE 5.0
#define WORLDUNITSIZE 100.0
#define ZFLOORSIZE 500.0
#define ZOOMSIZE 500.0
#define ARMDEFAULTLENGTH 1000.0
#define BUILDPLANE ECC_GameTraceChannel1
#define SHIPSEGMENT ECC_GameTraceChannel2
#define SHIPSEGINVALID ECC_GameTraceChannel3
#define PLACEDHULL ECC_GameTraceChannel4
#define ZFLOORHALF ZFLOORSIZE/2
#define ZFLOORSCALE 5
#define DENSITYEMPTY 0.0
#define ALTDEFAULT -999999

/*************************** These ENUMS AND STRUCTS should be moved into their own definition file that is exclusive to ShipBuilding and Templates*************************************/
//This Enum is used to indicate which pawn mode the player is currently in
enum PawnMode {
	character,
	shipbuilder,
	starship,
	turret,
	pilot
};

// This Enum is used to indicate which Ship Build mode the player is in
UENUM()
enum BuildMode {
	bmhull UMETA(DisplayName = "Hulls"),
	bmitem UMETA(DisplayName = "Items"),
	bmroom UMETA(DisplayName = "Rooms"),
	bmselect UMETA(DisplayName = "Select"),
	bmrepair UMETA(DisplayName = "Repair"),
	bmdestroy UMETA(DisplayName = "Destroy")
};

// Enum indicates the current drag type for ship building segments
UENUM()
enum SegmentType {
	cube UMETA(DisplayName = "Cube"),
	cylinder UMETA(DisplayName = "Cylinder"),
	sphere UMETA(DisplayName = "Sphere")
};

// Enum indicates which data asset to stamp into the voxel world 
enum AStampType {
	stampCube,
	stampWedge,
	stampCorner,
	stampCubeAngle,
	stampAngled,
	stampInvertedWedge,
	stampInvertedCorner
};

// Enum indicates item types to place
UENUM()
enum AItemType {
	itemWindow UMETA(DisplayName = "Window"),
	itemStairs UMETA(DisplayName = "Stairs"),
	itemDoor UMETA(DisplayName = "Door")
};

// Struct contained in drag segments and drag data as an array/map to identify build locations and stamp types
struct APieceData {
	AStampType StampType;
	FTransform Transform;
};

// Struct contaning all pertinent ship drag segment data
struct ADragData {
	FVector Location;
	FRotator Rotation;
	FVector Extent;
	TMap<FVector, APieceData> PieceMap;
	TMap<FVector, APieceData> HISMData;
	bool OffsetAdded;
};



