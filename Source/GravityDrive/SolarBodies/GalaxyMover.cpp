// ©, 2021, Erik Jastad. All Rights Reserved


#include "GalaxyMover.h"

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
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Velocity %f %f %f"), CurrentVelocity.X, CurrentVelocity.Y, CurrentVelocity.Z));
	CurrentVelocity *= 0.99;
}

void AGalaxyMover::AddVelocity(FVector ToAdd)
{
	CurrentVelocity += ToAdd;
}