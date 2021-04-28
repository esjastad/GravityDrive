// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#define XYSIZE 50.0
#define ZSIZE 5.0
#define WORLDUNITSIZE 100.0
#define ZFLOORSIZE 500.0
#define ZOOMSIZE 500.0
#define ARMDEFAULTLENGTH 1000.0
#define BUILDPLANE ECC_GameTraceChannel1
#define SHIPSEGMENT ECC_GameTraceChannel2
#define SHIPSEGINVALID ECC_GameTraceChannel3
#define ZFLOORHALF ZFLOORSIZE/2
#define ZFLOORSCALE 5
#define DENSITYEMPTY 0.0

//This Enum is used to indicate which pawn mode the player is currently in
enum PawnMode {
	character,
	shipbuilder,
	starship,
	turret,
	pilot
};

enum SegmentType {
	cube,
	cylinder,
	sphere
};

enum AStampType {
	stampCube,
	stampWedge,
	stampCorner,
	stampCubeAngle,
	stampAngled,
	stampInvertedWedge,
	stampInvertedCorner
};

struct APieceData {
	AStampType StampType;
	FTransform Transform;
};

struct ADragData {
	FVector Location;
	FRotator Rotation;
	FVector Extent;
	TMap<FVector, APieceData> PieceMap;
	TMap<FVector, APieceData> HISMData;
	bool OffsetAdded;
};
