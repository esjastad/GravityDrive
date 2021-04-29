// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipSegmentPlaced.h"
#include "Kismet/KismetMathLibrary.h"

AShipSegmentPlaced::AShipSegmentPlaced() : AShipSegment()
{
	Area = CreateDefaultSubobject<UBoxComponent>("Area");
	Area->SetBoxExtent(FVector(350.0, 350.0, 300.0));
	Area->SetupAttachment(RootComponent);
	Area->SetCollisionResponseToChannel(SHIPSEGMENT, ECollisionResponse::ECR_Overlap);

	InvalidArea = CreateDefaultSubobject<UBoxComponent>("AreaInvalid");
	InvalidArea->SetBoxExtent(FVector(250.0, 250.0, 200.0));
	InvalidArea->SetupAttachment(RootComponent);
	InvalidArea->SetCollisionResponseToChannel(SHIPSEGINVALID, ECollisionResponse::ECR_Block);
	
}

void AShipSegmentPlaced::SetDragData(ADragData * NewData, AHISMManager * NewHISMManager)
{
	DragData.Extent = FVector(NewData->Extent.X, NewData->Extent.Y, NewData->Extent.Z);
	DragData.Location = FVector(NewData->Location.X, NewData->Location.Y, NewData->Location.Z);
	DragData.Rotation = FRotator(NewData->Rotation);
	DragData.PieceMap.Append(NewData->PieceMap);
	DragData.HISMData.Append(NewData->HISMData);
	DragData.OffsetAdded = NewData->OffsetAdded;
	
	SetActorLocationAndRotation(DragData.Location, DragData.Rotation);
	Area->SetBoxExtent(DragData.Extent + XYSIZE);
	InvalidArea->SetBoxExtent(DragData.Extent - WORLDUNITSIZE);
	HISMManager = NewHISMManager;
	FVector UNRCenter = UKismetMathLibrary::Quat_UnrotateVector(FQuat(DragData.Rotation), DragData.Location);
	UNRCenter += (DragData.OffsetAdded) ? FVector(0,0,-ZFLOORHALF) : FVector(0, 0, -ZFLOORHALF);
	//HISMManager->AddHullHISM(&DragData.PieceMap, &UNRCenter);

}

void AShipSegmentPlaced::MouseOver(int HitCount, float ZLevel)
{
	TArray<FTransform> XForm;
	FVector Extent = DragData.Extent;
	FVector Center = UKismetMathLibrary::Quat_UnrotateVector(FQuat(GetActorRotation()), Area->GetComponentLocation());
	FVector PCE = Center + Extent;
	FVector NCE = Center - Extent;
	
	FVector PPLoc = FVector(PCE.X, PCE.Y, ZLevel);
	FVector PNLoc = FVector(PCE.X, NCE.Y, ZLevel);
	FVector NPLoc = FVector(NCE.X, PCE.Y, ZLevel);
	FVector NNLoc = FVector(NCE.X, NCE.Y, ZLevel);

	XForm.Add(FTransform(FRotator(0), PPLoc, FVector(1.005)));
	XForm.Add(FTransform(FRotator(0), PNLoc, FVector(1.005)));
	XForm.Add(FTransform(FRotator(0), NPLoc, FVector(1.005)));
	XForm.Add(FTransform(FRotator(0), NNLoc, FVector(1.005)));
	XForm.Add(FTransform(FRotator(0), FVector(Center.X, Center.Y, ZLevel), FVector(1.005)));
	XForm.Add(FTransform(FRotator(0), FVector(PPLoc.X, Center.Y, ZLevel), FVector(1.005)));
	XForm.Add(FTransform(FRotator(0), FVector(NNLoc.X, Center.Y, ZLevel), FVector(1.005)));
	XForm.Add(FTransform(FRotator(0), FVector(Center.X, PPLoc.Y, ZLevel), FVector(1.005)));
	XForm.Add(FTransform(FRotator(0), FVector(Center.X, NNLoc.Y, ZLevel), FVector(1.005)));

	HISMManager->Anchors(&XForm,HitCount);
	
}

bool AShipSegmentPlaced::GetBuildNext(APieceData * NextBuildPiece)
{
	TArray<APieceData> Values;
	DragData.PieceMap.GenerateValueArray(Values);
	bool valid = false;

	if (Values.IsValidIndex(CBIndex))
	{
		NextBuildPiece->StampType = Values[CBIndex].StampType;
		NextBuildPiece->Transform = FTransform(Values[CBIndex].Transform.GetRotation(), Values[CBIndex].Transform.GetLocation(), FVector(1));
		++CBIndex;
		valid = true;
	}
	
	return valid;

}