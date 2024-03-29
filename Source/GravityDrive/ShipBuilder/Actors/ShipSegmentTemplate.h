// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ShipSegmentPlaced.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
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
		void Update(FVector Location, FVector Scale, bool Valid, FRotator newRotation);
		void MirrorUpdate(FVector Location, FVector Scale, bool Valid, FRotator newRotation, TArray<FTransform> HXForm[5]);
		void Initialize(SegmentType type, FVector Mirror, bool IsOriginal);
		virtual void BeginPlay() override;
		void FillTransform(TArray<FTransform> * XForm, FVector * Scale);
		void PackageDrag(ADragData * Data, AHISMManager * HManager, AVoxelWorld * ShipRef, AActor * ShipManagerRef);
		void MirrorPackage(ADragData * Data, AHISMManager * HManager, AVoxelWorld * ShipRef, AActor * ShipManagerRef);
		virtual void Destroyed();
		/***********************************************************************************************************************REDO REFERENCES set in BP*/
		SegmentType CurrentType;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
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

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UMaterialInstance * MatGuide;

		UMaterialInstanceDynamic* Material;
		ADragData DragData;

		USceneComponent * Guide;
		UStaticMeshComponent * XLength;
		UStaticMeshComponent * YLength;
		UStaticMeshComponent * ZLength;

		TArray<AShipSegmentTemplate*> MyMirrors;
		TArray<FVector> MirrorDirections;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<AShipSegmentPlaced> SegPlacedClass;
};
