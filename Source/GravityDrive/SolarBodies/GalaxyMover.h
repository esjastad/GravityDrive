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
	virtual void InstanceSwap();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent * SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent * Mover;

	void AddVelocity(FVector ToAdd);
	FVector CurrentVelocity;
	FVector CurrentLocation;
	FVector LastStarCheckLocation;
};
