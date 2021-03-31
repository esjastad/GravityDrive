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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Component Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Cursor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Scene;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
