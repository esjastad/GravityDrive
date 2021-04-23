// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/ShipSegmentTemplate.h"
#include "HISMManager.generated.h"

UCLASS()
class GRAVITYDRIVE_API AHISMManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHISMManager();

	USceneComponent * Scene;
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

	UMaterialInstanceDynamic* Material;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInstance * AnchorMatRef;
	UMaterialInstanceDynamic* AnchorMaterial;

	void Anchors(FBox * Bounds);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void AddHullHISM(TMap<FVector, APieceData> * HISMData);
};
