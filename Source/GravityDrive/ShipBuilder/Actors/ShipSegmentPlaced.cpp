// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipSegmentPlaced.h"
#include "VoxelTools/Gen/VoxelBoxTools.h"
#include "VoxelGenerators/VoxelGeneratorInstanceWrapper.h"

#include "VoxelTools/VoxelAssetTools.h"
#include "VoxelGenerators/VoxelGeneratorTools.h"
#include "VoxelTools/VoxelToolHelpers.h"
#include "VoxelData/VoxelDataIncludes.h"
#include "VoxelTools/VoxelDataTools.h"
#include "VoxelAssets/VoxelDataAssetData.inl"

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

void AShipSegmentPlaced::SetDragData(ADragData * NewData, AHISMManager * NewHISMManager, AVoxelWorld * ShipRef)
{
	DragData.Extent = FVector(NewData->Extent.X, NewData->Extent.Y, NewData->Extent.Z);
	DragData.Location = FVector(NewData->Location.X, NewData->Location.Y, NewData->Location.Z);
	DragData.Rotation = FRotator(NewData->Rotation);
	DragData.PieceMap.Append(NewData->PieceMap);
	DragData.HISMData.Append(NewData->HISMData);
	DragData.OffsetAdded = NewData->OffsetAdded;
	
	FRotator RotToBe = GetAttachParentActor()->GetActorRotation();
	//SetActorLocationAndRotation(UKismetMathLibrary::Quat_RotateVector(FQuat(), DragData.Location), DragData.Rotation);
	SetActorLocationAndRotation(UKismetMathLibrary::Quat_RotateVector(FQuat(RotToBe), DragData.Location), RotToBe);
	Area->SetBoxExtent(DragData.Extent + XYSIZE);
	InvalidArea->SetBoxExtent(DragData.Extent - WORLDUNITSIZE);
	HISMManager = NewHISMManager;
	FVector UNRCenter = UKismetMathLibrary::Quat_UnrotateVector(FQuat(DragData.Rotation), DragData.Location);
	UNRCenter += (DragData.OffsetAdded) ? FVector(0,0,-ZFLOORHALF) : FVector(0, 0, -ZFLOORHALF);
	//HISMManager->AddHullHISM(&DragData.PieceMap, &UNRCenter);
	VoxelShipRef = ShipRef;	// Assign the voxel ship reference

	TArray<FHitResult> HitResults;
	FCollisionShape CollisionShape;
	FVector CenterLoc = DragData.Location;// +DragData.Extent;
	//Adjust the Center and Half Extent if drag is positive in Z
	
	CollisionShape.ShapeType = ECollisionShape::Box;
	CollisionShape.SetBox(DragData.Extent + 50);
	//Rotate the center vector to align with the current world rotation
	CenterLoc = UKismetMathLibrary::Quat_RotateVector(FQuat(DragData.Rotation), CenterLoc);

	//This function returns true only if the hit was a block response, since these are overlap hits, the results array needs to be checked to determine if a hit was made.
	GetWorld()->SweepMultiByChannel(HitResults, CenterLoc - 1, CenterLoc + 1, FQuat(DragData.Rotation), SHIPSEGMENT, CollisionShape);
	
	for(auto &elem : HitResults)	// Go through results and add them to touched array, signal to them as well to add us!
	{
		if (elem.Actor != this)
		{
			AShipSegmentPlaced * SegToAdd = Cast<AShipSegmentPlaced>(elem.Actor);
			if (SegToAdd)
			{
				TouchedSegments.AddUnique(SegToAdd);
				SegToAdd->AddTouchingSegment(this);
			}
		}
	}
	//DrawDebugBox(GetWorld(), CenterLoc, DragData.Extent, FColor::Green, false, 1);
}

void AShipSegmentPlaced::AddTouchingSegment(AShipSegmentPlaced * SegToAdd)
{
	TouchedSegments.AddUnique(SegToAdd);
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

// Destroys all internal placed items and notifies neighbors to redo placed walls
void AShipSegmentPlaced::UserDestroy()
{
	FRotator PROT = GetAttachParentActor()->GetActorRotation();

	FVector RegMin = UKismetMathLibrary::Quat_RotateVector(FQuat(PROT), (DragData.Location - (DragData.Extent + 75)).GridSnap(1));
	FVector RegMax = UKismetMathLibrary::Quat_RotateVector(FQuat(PROT), (DragData.Location + (DragData.Extent + 125)).GridSnap(1));

	FIntVector min = VoxelShipRef->GlobalToLocal(RegMin, EVoxelWorldCoordinatesRounding::RoundDown);
	FIntVector max = VoxelShipRef->GlobalToLocal(RegMax, EVoxelWorldCoordinatesRounding::RoundUp);
	FVoxelIntBox BoxToRemove = FVoxelIntBox(min,max);
	UVoxelBoxTools::RemoveBox(VoxelShipRef, BoxToRemove);

	for (auto &elem : TouchedSegments)
	{
		elem->RemoveTouchingSegment(this);
	}
	this->Destroy();
}

void AShipSegmentPlaced::RemoveTouchingSegment(AShipSegmentPlaced * SegToRemove)
{
	TouchedSegments.Remove(SegToRemove);	// Remove the segment from touched list
	FVector SegDiffDirection(SegToRemove->DragData.Location - DragData.Location);		// Find direction of touch
	
	SegDiffDirection.Normalize(); // Get direction unit vector
	SegDiffDirection.X = ceil(SegDiffDirection.X);	// Ceiling the results because we want the max extent since thats the only overlap that is allowed
	SegDiffDirection.Y = ceil(SegDiffDirection.Y);
	SegDiffDirection.Z = ceil(SegDiffDirection.Z);

	FVector MyMax(DragData.Location + DragData.Extent);		// Get My Max and Min bounds
	FVector MyMin(DragData.Location - DragData.Extent);
	FVector TheirMax(SegToRemove->DragData.Location + SegToRemove->DragData.Extent); // Get their Max and min bounds
	FVector TheirMin(SegToRemove->DragData.Location - SegToRemove->DragData.Extent);

	FVector OverlapMax = FVector( fmin(MyMax.X, TheirMax.X), fmin(MyMax.Y, TheirMax.Y), fmin(MyMax.Z,TheirMax.Z));		// Get overlap max by taking min of Maxes
	FVector OverlapMin = FVector( fmax( MyMin.X, TheirMin.X ), fmax(MyMin.Y, TheirMin.Y), fmax(MyMin.Z, TheirMin.Z));	// Get overlap min by taking max of Mins
	
	FVector OverlapExtent = (OverlapMax - OverlapMin) * 0.5;	// Get the overlap extent of the two overlapping boxes
	FVector OverlapCenter = OverlapMin + OverlapExtent; // Get the overlap center of the two overlapping boxes

	
	TArray<APieceData> Pieces;	// Array to hold Map Values
	DragData.PieceMap.GenerateValueArray(Pieces);	// Get Map Values
	
	for (auto &elem : Pieces)	// Iterate through map values
	{
		/*FTransform PForm = elem.Transform;	// Get the transform of the current element
		FVector PLoc = PForm.GetLocation() + (DragData.Location -  ((DragData.OffsetAdded) ? FVector(0,0,ZFLOORHALF) : FVector(0)));	// Get and adjust the element location to match world loc instead of loca
		PForm.SetLocation(PLoc);	// Set the transform to the adjust location
		if (UKismetMathLibrary::IsPointInBox(PLoc, OverlapCenter, OverlapExtent))	// If elements world location is inside of the overlapping box then stamp it back into the world
		{
			AddStampToVoxelShip(elem.StampType, &PForm);
		}*/
		
		FTransform PForm = elem.Transform;	// Get the transform of the current element
		TransformLocToWorld(&PForm);

		//FVector PLoc = PForm.GetLocation() + (DragData.Location - ((DragData.OffsetAdded) ? FVector(0, 0, ZFLOORHALF) : FVector(0)));	// Get and adjust the element location to match world loc instead of loca
		//PForm.SetLocation(PLoc);	// Set the transform to the adjust location
		if (UKismetMathLibrary::IsPointInBox(PForm.GetLocation(), OverlapCenter, OverlapExtent))	// If elements world location is inside of the overlapping box then stamp it back into the world
		{
			AddStampToVoxelShip(elem.StampType, elem.Transform);
		}
	}
}

void AShipSegmentPlaced::TransformLocToWorld(FTransform * TForm)
{
	FVector WLoc = TForm->GetLocation() + (DragData.Location - ((DragData.OffsetAdded) ? FVector(0, 0, ZFLOORHALF) : FVector(0)));	// Get and adjust the element location to match world loc instead of loca
	TForm->SetLocation(WLoc);
}

void AShipSegmentPlaced::AddStampToVoxelShip(AStampType Stamp, FTransform TForm)
{
	TransformLocToWorld(&TForm);

	
	FRotator PROT = GetAttachParentActor()->GetActorRotation();
	FVector RLoc = UKismetMathLibrary::Quat_RotateVector(FQuat(PROT), TForm.GetLocation().GridSnap(1)); 
	TForm.SetLocation(RLoc);

	
	FRotator TROT = FRotator(TForm.GetRotation());

	//Find Rotator to use by rotating these axis and making a rotator from them
	FVector UpVec = UKismetMathLibrary::Quat_RotateVector(FQuat(TROT), FVector(0, 0, 1)).GridSnap(1);
	UpVec = UKismetMathLibrary::Quat_RotateVector(FQuat(PROT), UpVec);
	FVector Right = UKismetMathLibrary::Quat_RotateVector(FQuat(TROT), FVector(0, 1, 0)).GridSnap(1);
	Right = UKismetMathLibrary::Quat_RotateVector(FQuat(PROT), Right);
	FVector Front = UKismetMathLibrary::Quat_RotateVector(FQuat(TROT), FVector(1, 0, 0)).GridSnap(1);
	Front = UKismetMathLibrary::Quat_RotateVector(FQuat(PROT), Front);

	//Set Rotator 
	FRotator WHAT = UKismetMathLibrary::MakeRotationFromAxes(Front, Right, UpVec);
	TForm.SetRotation(FQuat(WHAT));

	switch (Stamp) {
	case AStampType::stampCube:
	{
		UVoxelTransformableGeneratorInstanceWrapper * InnerCubeWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(CubeInner, VoxelShipRef->GetGeneratorInit());
		UVoxelTransformableGeneratorInstanceWrapper * CubeWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(Cube, VoxelShipRef->GetGeneratorInit());
		UVoxelAssetTools::ImportAsset(VoxelShipRef, CubeWrapper, TForm, FVoxelIntBox());
	}
	break;
	case AStampType::stampWedge:
	{
		UVoxelTransformableGeneratorInstanceWrapper * WedgeWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(Wedge, VoxelShipRef->GetGeneratorInit());
		UVoxelAssetTools::ImportAsset(VoxelShipRef, WedgeWrapper, TForm, FVoxelIntBox());
	}
	break;
	case AStampType::stampCorner:
	{
		UVoxelTransformableGeneratorInstanceWrapper * CornerWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(Corner, VoxelShipRef->GetGeneratorInit());
		UVoxelAssetTools::ImportAsset(VoxelShipRef, CornerWrapper, TForm, FVoxelIntBox());
	}
	break;
	case AStampType::stampInvertedWedge:
	{
		UVoxelTransformableGeneratorInstanceWrapper * InvertedWedgeWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(InvertedWedge, VoxelShipRef->GetGeneratorInit());
		UVoxelAssetTools::ImportAsset(VoxelShipRef, InvertedWedgeWrapper, TForm, FVoxelIntBox());
	}
	break;
	case AStampType::stampInvertedCorner:
	{
		UVoxelTransformableGeneratorInstanceWrapper * InvertedCornerWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(InvertedCorner, VoxelShipRef->GetGeneratorInit());
		UVoxelAssetTools::ImportAsset(VoxelShipRef, InvertedCornerWrapper, TForm, FVoxelIntBox());
	}
	break;
	case AStampType::stampAngled:
	{
		UVoxelTransformableGeneratorInstanceWrapper * AngledWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(Angled, VoxelShipRef->GetGeneratorInit());
		UVoxelAssetTools::ImportAsset(VoxelShipRef, AngledWrapper, TForm, FVoxelIntBox());
	}
	break;
	case AStampType::stampCubeAngle:
	{
		UVoxelTransformableGeneratorInstanceWrapper * CubeAngledWrapper = UVoxelGeneratorTools::MakeTransformableGeneratorInstance(CubeAngled, VoxelShipRef->GetGeneratorInit());
		UVoxelAssetTools::ImportAsset(VoxelShipRef, CubeAngledWrapper, TForm, FVoxelIntBox());
	}
	break;
	}
}
