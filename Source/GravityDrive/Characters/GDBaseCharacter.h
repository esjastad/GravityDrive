// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
#include "GravityMovementComponent.h"
#include "GravityCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GDBaseCharacter.generated.h"

/*
 * A Gravity Drive Character Class, this is the base class for all game characters
 */
UCLASS()
class GRAVITYDRIVE_API AGDBaseCharacter : public AGravityCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGDBaseCharacter(const FObjectInitializer& ObjectInitializer);
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnRight(float Value);
	void LookUp(float Value);
	void GravityUpdate(FVector GravityCenter);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector GravityDir;
	APlayerController * PC;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Add SpringArm and Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* CamSpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* OTSCamera;
	AActor * GravityActor;
};
