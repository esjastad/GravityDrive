// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Actors/ShipSegmentTemplate.h"
#include "../Definitions.h"
#include "ShipBuildTools.generated.h"


/**
 * Tool manager for shipmanager, contains the cursor, axis and grid
 */
UCLASS()
class GRAVITYDRIVE_API AShipBuildTools : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShipBuildTools();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Update(FHitResult* NewHit, bool IsValid, BuildMode CurrentMode, AItemType CurrentItem);
	void GetLocationRotation(FVector * Loc, FRotator * Rot);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Updates state variables
	void InitState();
	void UpdateState(FVector* NewLoc, BuildMode CurrentMode, AItemType CurrentItem);
	
	// Component Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Cursor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* XAxis;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* YAxis;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Grid;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Scene;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialParameterCollection* MPCAsset;

	UMaterialInstanceDynamic* Material;
	UMaterialParameterCollectionInstance* MPC;

	FVector Location;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh* BuildC;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh* WindowC;

};
