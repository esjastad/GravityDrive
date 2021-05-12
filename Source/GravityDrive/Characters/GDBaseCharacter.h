// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GravityMovementComponent.h"
#include "GravityCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GDBaseCharacter.generated.h"

/*
 * A Gravity Drive Character Class, this is the base class for all game characters
 * Includes a directed gravity component for orienting towards a new gravity.
 */
UCLASS()
class GRAVITYDRIVE_API AGDBaseCharacter : public AGravityCharacter
{
	GENERATED_BODY()

public:
	
	AGDBaseCharacter(const FObjectInitializer& ObjectInitializer);		// Sets default values for this character's properties
	virtual void Tick(float DeltaTime) override;		// Called every frame
	virtual void PossessedBy(AController* NewController) override;		// Called when possessed
	virtual void UnPossessed() override;	// Called when unpossessed
	void MoveForward(float Value);		// Controls Forward/Backward movement
	void MoveRight(float Value);		// Controls Right/Left movement
	void TurnRight(float Value);		// Controls Turning/Looking Right/Left
	void LookUp(float Value);		// Controls Turning/Looking Up/Down
	void GravityUpdate(FVector GravityCenter);		// Updates the gravity orientation based on a "normal" vector input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;	// Called when possessed to bind functionality to inputs

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector GravityDir;		// Blueprint exposed vector that indicates what the GravityDir is/should be
	APlayerController * PC;		// Reference to a base class player controller

protected:
	virtual void BeginPlay() override;		// Called when the game starts or when spawned

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* CamSpringArm;		// Add SpringArm and expose it in BP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* OTSCamera;		// Add Camera and expose it in BP
	AActor * GravityActor;		// Reference to gravity creating actor, such as the SHIP or a PLANET
};
