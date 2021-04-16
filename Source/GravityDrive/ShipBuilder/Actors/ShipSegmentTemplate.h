// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ShipSegment.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "DragTemplates.h"
#include "ShipSegmentTemplate.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITYDRIVE_API AShipSegmentTemplate : public AShipSegment
{
	GENERATED_BODY()
	public:
		AShipSegmentTemplate();
		void Update(FVector Location, FVector Scale);
		void Initialize(SegmentType type);
		virtual void BeginPlay() override;
		void FillTransform(TArray<FTransform> * XForm, FVector * Scale);
		void PackageDrag(ADragData * Data);

		SegmentType CurrentType;
		
		TArray<UHierarchicalInstancedStaticMeshComponent*> HISMList;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh * Cube;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh * Wedge;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh * Corner;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh * CubeAngle;
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh * Angled;
		ADragData DragData;
};
