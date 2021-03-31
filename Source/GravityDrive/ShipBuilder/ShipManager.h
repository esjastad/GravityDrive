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
#include "UI/ShipBuildWidget.h"
#include "ShipBuildTools.h"
#include "VoxelWorld.h"
#include "ShipManager.generated.h"

/*
 *ShipManager class, handles ship part building and destruction
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

	void MoveForward(float Value);
	void MoveRight(float Value);
	void ZoomIn();
	void ZoomOut();
	void UpZ();
	void DownZ();

	// Class references
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AShipBuildTools> cBuildToolClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AShipBuildTools* cBuildTools;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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

};
