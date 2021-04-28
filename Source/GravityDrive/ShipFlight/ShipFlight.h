// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "VoxelWorld.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "../ShipBuilder/ShipManager.h"
#include "GameFramework/Pawn.h"
#include "ShipFlight.generated.h"

UCLASS()
class GRAVITYDRIVE_API AShipFlight : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AShipFlight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	USceneComponent * Scene;
	APlayerController* PController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AVoxelWorld * VoxelShip;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* Camera;
	
	ARecastNavMesh * RNavMesh;
	ANavMeshBoundsVolume * NavMeshBV;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialParameterCollection* MPCAsset;
	UMaterialParameterCollectionInstance* MPC;
};
