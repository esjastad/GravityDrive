// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GalaxyMover.generated.h"

UCLASS()
class GRAVITYDRIVE_API AGalaxyMover : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGalaxyMover();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Implemented in Child but called in this class
	virtual void InstanceSwap();	// Swap out HISM for a SolarBody Spawn
	virtual void UpdateLight();		// Update star light if spawned star exists

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent * SceneRoot;	// The Root component does not move
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent * Mover;	//This is moved around to simulate flying, all bodies are attached to this, this actor class does not move

	void AddVelocity(FVector ToAdd);	//Add Velocity 
	FVector CurrentVelocity;		// Our current Velocity
	FVector CurrentLocation;		// Where we are in the world currentl
	FVector LastStarCheckLocation;	// Used to calc distance for doing a new star check
};
