// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "ShipBuildTools.generated.h"

/**
 * Tool manager for shipmanager
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
	void Update(FHitResult* NewHit, bool IsValid);
	void GetLocation(FVector * Loc);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Updates state variables
	void InitState();
	void UpdateState(FVector* NewLoc);
	
	// Component Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Cursor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Scene;

	UMaterialInstanceDynamic* Material;

	FVector Location;

};
