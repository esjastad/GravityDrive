// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipSegmentPlaced.h"

AShipSegmentPlaced::AShipSegmentPlaced() : AShipSegment()
{
	Area = CreateDefaultSubobject<UBoxComponent>("Area");
	Area->SetBoxExtent(FVector(350.0, 350.0, 300.0));
	Area->SetupAttachment(RootComponent);
}

void AShipSegmentPlaced::SetDragData(ADragData * NewData)
{
	DragData.Extent = FVector(NewData->Extent.X, NewData->Extent.Y, NewData->Extent.Z);
	DragData.Location = FVector(NewData->Location.X, NewData->Location.Y, NewData->Location.Z);
	DragData.PieceMap.Append(NewData->PieceMap);
	SetActorLocation(DragData.Location);
	Area->SetBoxExtent(DragData.Extent + XYSIZE);
}

void AShipSegmentPlaced::GetBuildNext(APieceData * NextBuildPiece)
{
	TArray<APieceData> Values;
	DragData.PieceMap.GenerateValueArray(Values);

	NextBuildPiece->StampType = Values[CBIndex].StampType;
	NextBuildPiece->Transform = FTransform(Values[CBIndex].Transform.GetRotation(), Values[CBIndex].Transform.GetLocation(), FVector(1));
	++CBIndex;

	/*for (auto& Elem : DragData.PieceMap)
	{
		switch (Elem.Value.StampType) {
		case AStampType::stampCube:
			break;
		case AStampType::stampWedge:
			break;
		case AStampType::stampCorner:
			break;
		}
	}*/
}