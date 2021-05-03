// ©, 2021, Erik Jastad. All Rights Reserved


#include "GalaxyMover.h"
#include "SolarBodyDefinitions.h"

// Sets default values
AGalaxyMover::AGalaxyMover()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	RootComponent = SceneRoot;

	Mover = CreateDefaultSubobject<USceneComponent>("Mover");
	Mover->SetupAttachment(RootComponent);

	CurrentVelocity = FVector(0);
}

// Called when the game starts or when spawned
void AGalaxyMover::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGalaxyMover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Mover->AddLocalOffset(CurrentVelocity);
	CurrentLocation += CurrentVelocity;

	FVector Garbage;
	float Distance;
	(CurrentLocation - LastStarCheckLocation).ToDirectionAndLength(Garbage,Distance);
	// If we have moved far enoough, check for a star to instance swap
	if (Distance > SOLARSYSTEMAVERAGE)
	{
		LastStarCheckLocation = CurrentLocation;
		InstanceSwap();
	}
	// Slow down over time mechanic
	CurrentVelocity *= 0.99;
}

void AGalaxyMover::InstanceSwap() {}

void AGalaxyMover::AddVelocity(FVector ToAdd)
{
	CurrentVelocity += ToAdd;
}