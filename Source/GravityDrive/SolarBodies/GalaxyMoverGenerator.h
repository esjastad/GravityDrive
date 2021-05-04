// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GalaxyMover.h"
#include "SolarBody.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "../ShipFlight/ShipFlight.h"
#include "GalaxyMoverGenerator.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITYDRIVE_API AGalaxyMoverGenerator : public AGalaxyMover
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AGalaxyMoverGenerator();
	void GenerateCenter();
	void GenerateStars();
	void UpdateLight();

	int StarCheck(int * HISMHit, FVector Loc = FVector(0));
	void InstanceSwap();
	float GPhi(float R, float Phi, float Val);
	int MakeWeightedStarSpawnIndex();
	FVector StartingLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASolarBody> SolarBodyBP;
	FRandomStream GalaxyGenStream;

	ASolarBody * SpawnedStar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* StarMesh;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance * OStarMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance * BStarMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance * AStarMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance * FStarMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance * GStarMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance * KStarMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance * MStarMat;*/

	//Keep!
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UHierarchicalInstancedStaticMeshComponent*> HISMList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMaterialInstance*> StarMatList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector2D> StarScalesList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDirectionalLightComponent* StarLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AShipFlight * ShipFlightRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialParameterCollection* MPCAsset;
	UMaterialParameterCollectionInstance* MPC;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
