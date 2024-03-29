// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "../Definitions.h"
//#include "Actors/ShipSegmentTemplate.h"
#include "HISMManager.generated.h"

/*
 * HISM Manager, manages all placed ship HISMS
 *
 */
UCLASS()
class GRAVITYDRIVE_API AHISMManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHISMManager();
	//**********************************************************************************************************************REDO References so they can more easily be in BP**
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh * Window;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh * Door;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh * Stairs;

	UMaterialInstanceDynamic* Material;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInstance * AnchorMatRef;
	UMaterialInstanceDynamic* AnchorMaterial;

	void Anchors(TArray<FTransform> * XForm, int CallsTillReset);
	void AnchorClear();
	void AddItem(AItemType type, FTransform TForm);
	FTimerHandle AnchorClearTimer;
	int AnchorCallCount;

	TArray<UHierarchicalInstancedStaticMeshComponent*> ItemHISMList;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void AddHullHISM(TMap<FVector, APieceData> * HISMData, FVector * Center);
};
