// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../VoxelShip/VoxelShip.h"
#include "ShipBuildTools.h"
//#include "VoxelWorld.h"
//#include "Actors/ShipSegmentTemplate.h"
//#include "VoxelAssets/VoxelDataAsset.h"
#include "ShipManager.generated.h"

/*
 * ShipManager class, handles ship part building and destruction
 */
UCLASS()
class GRAVITYDRIVE_API AShipManager : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AShipManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	void LookUp(float Value);
	void TurnRight(float Value);
	void MoveForward(float Value);
	void MoveRight(float Value);
	void ZoomIn();
	void ZoomOut();
	void UpZ();
	void DownZ();
	void HandleLeftClick();
	void HandleLeftRelease();
	void HandleRightClick();
	void UpdateArmLength(float DTime);
	void UpdateSegType(SegmentType * newType);
	void UpdateCanRotate();
	void UpdateBuildMode(BuildMode newMode);
	void UpdateMirrorMode(FVector newMode);
	void AddPlacedSegment(AShipSegmentPlaced * NewPlacedSeg);
	void UpdateItemMode(AItemType newMode);

	// Class references
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AShipSegmentTemplate> SegTemplateClass;
	AShipSegmentTemplate* SegTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AShipBuildTools> cBuildToolClass;
	AShipBuildTools* cBuildTools;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AShipSegmentPlaced> SegPlacedClass;
	TArray<AShipSegmentPlaced*> PlacedSegments;
	//AShipSegmentPlaced * NewPlacedSeg;

	//Voxel Plugin Reliant
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AVoxelWorld * VoxelShip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AHISMManager * HISMManager;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool ValidateDrag(FVector HalfExtent, FVector Start);

	// Add Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFloatingPawnMovement* Movement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Scene;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialParameterCollection* MPCAsset;
	UMaterialParameterCollectionInstance* MPC;

	AShipSegment* SelectedSegment;
	APlayerController* PController;
	FVector DragStart;
	FRotator DragRotation;
	FVector TraceLoc;
	FVector CurrentLoc;
	FVector MirrorMode;	// 1 = on, 0 = off for each respective axis
	FRotator CurrentRotation;
	int ShipLevel;
	int UBSIndex;
	float ArmTarget;
	bool ValidDrag;
	bool LMB;
	bool bMMB;
	SegmentType SegType;
	BuildMode CurrentBuildMode;
	AItemType CurrentItemMode;
	FHitResult CursorHit;		// Tick hit result stored here
};
