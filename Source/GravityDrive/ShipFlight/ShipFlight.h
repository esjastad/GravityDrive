// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "VoxelWorld.h"
#include "QuatRotations.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "../ShipBuilder/ShipManager.h"
#include "../SolarBodies/GalaxyMover.h"
#include "GameFramework/Pawn.h"
#include "ShipFlight.generated.h"

/*
 * Class for Ship flight controls, extends from Pawn
*/
UCLASS()
class GRAVITYDRIVE_API AShipFlight : public APawn
{
	GENERATED_BODY()

public:
	
	AShipFlight();		// Sets default values for this pawn's properties

protected:
	
	virtual void BeginPlay() override;		// Called when the game starts or when spawned

public:	
	
	virtual void Tick(float DeltaTime) override;		// Called every frame

	virtual void PossessedBy(AController* NewController) override;		// Called when possessed
	virtual void UnPossessed() override;		// Called when unpossessed
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;		// Called to bind functionality to input

	void RollClockwise(float Value);	// Roll Right/Left based on value, positive is Clockwise
	void LookUp(float Value);		// Look Up/Down based on value pos/neg
	void TurnRight(float Value);	// Look Right/Left based on value pos/neg
	void MoveForward(float Value);	// Move Forward/Backward basedon value pos/neg
	void MoveRight(float Value);	// Move Right/Left based on value pos/neg
	void SpeedShift();		// Shift Speed based on Left Shift pressed or not

	USceneComponent * Scene;		// Root
	APlayerController* PController;	// Base Class Player Controller Reference

	UPROPERTY(EditAnywhere, BlueprintReadWrite)		// Voxels that represent Player Ship Reference
	AVoxelWorld * VoxelShip;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)	// Cam and SpringArm
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* Camera;
	
	ARecastNavMesh * RNavMesh;	//WIP Custom Navmesh to update with flight rotations, currently unused
	ANavMeshBoundsVolume * NavMeshBV; //WIP Custom Navmesh to update with flight rotations, currently unused

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)	// Material Collection Parameters
	UMaterialParameterCollection* MPCAsset;
	UMaterialParameterCollectionInstance* MPC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor * BAShipManager;		// Actor reference set from level reference

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// Galaxy reference, used to simulate movement forward back and left right 
	AGalaxyMover * FlightControl;

	float Speed;	// Controls max speed allowed aka MAXSPEED
	bool SolarSpeed;	// Controls limit on max speed allowed
};
