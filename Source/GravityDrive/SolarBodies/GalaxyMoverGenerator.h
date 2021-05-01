// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GalaxyMover.h"
#include "SolarBody.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
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
	float GFit(float A, float B, float N, float phi);
	float GPhi(float R, float Phi, float Val);
	FVector StartingLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASolarBody> SolarBodyBP;
	FRandomStream GalaxyGenStream;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* StarMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
	UMaterialInstance * MStarMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UHierarchicalInstancedStaticMeshComponent*> HISMList;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
