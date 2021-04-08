// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ShipSegment.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
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
		AShipSegmentTemplate(SegmentType type);
		void Update(FVector Location, FVector Scale);
		virtual void BeginPlay() override;

		SegmentType CurrentType;
		TArray<UHierarchicalInstancedStaticMeshComponent> MeshInstances;
};
