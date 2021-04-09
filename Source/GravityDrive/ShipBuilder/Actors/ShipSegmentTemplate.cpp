// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipSegmentTemplate.h"

AShipSegmentTemplate::AShipSegmentTemplate() : AShipSegment()
{
	CurrentType = SegmentType::cube;
	Scene = CreateDefaultSubobject<USceneComponent>("MyRoot");
	RootComponent = Scene;
	UHierarchicalInstancedStaticMeshComponent * NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Cube");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);
	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Wedge");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM);
	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Corner");
	NewHISM->SetupAttachment(RootComponent);
	HISMList.Add(NewHISM); 
}

// Called when the game starts or when spawned
void AShipSegmentTemplate::BeginPlay()
{
	Super::BeginPlay();
	HISMList[0]->SetStaticMesh(Cube);
	HISMList[1]->SetStaticMesh(Wedge);
	HISMList[2]->SetStaticMesh(Corner);
}

void AShipSegmentTemplate::Initialize(SegmentType type)
{
	CurrentType = type;	
	HISMList[0]->AddInstance(FTransform(FRotator(0.0), FVector(0, 0, 0), FVector(5,5,1)));
	HISMList[0]->AddInstance(FTransform(FRotator(0.0), FVector(0, 0, 500), FVector(5,5,1)));
	HISMList[0]->AddInstance(FTransform(FRotator(0.0), FVector(0, 250, 250), FVector(5,1,4)));
	HISMList[0]->AddInstance(FTransform(FRotator(0.0), FVector(0, -250, 250), FVector(5,1,4)));
	HISMList[0]->AddInstance(FTransform(FRotator(0.0), FVector(250, 0, 250), FVector(1,5,4)));
	HISMList[0]->AddInstance(FTransform(FRotator(0.0), FVector(-250, 0, 250), FVector(1,5,4)));
}

void AShipSegmentTemplate::Update(FVector Location, FVector Scale)
{
	float signX = (Scale.X > 0) ? signX = -1 : signX = 1;
	float signY = (Scale.Y > 0) ? signY = 1 : signY = -1;

	FVector DScale = Scale / 50;
	DScale = DScale.GetAbs();
	DScale.X = fmax(DScale.X - 1, 1);
	DScale.Y = fmax(DScale.Y - 1, 1);
	DScale.Z = (Scale.Z / 250.0) + 1;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Z is %f"), Scale.Z));
	SetActorLocation(Location);

	switch (CurrentType) {
		case cube:
			for (int i = 0; i < 3; ++i) {
				HISMList[i]->ClearInstances();
			}
			
			HISMList[0]->AddInstance(FTransform(FRotator(0.0), FVector(0, 0, 0), FVector(DScale.X, DScale.Y, 1)));
			HISMList[0]->AddInstance(FTransform(FRotator(0.0), FVector(0, 0, 500), FVector(DScale.X, DScale.Y, 1)));
			HISMList[0]->AddInstance(FTransform(FRotator(0.0), FVector(0, Scale.Y * signY, 250), FVector(DScale.X, 1, 4)));
			HISMList[0]->AddInstance(FTransform(FRotator(0.0), FVector(0, -Scale.Y * signY, 250), FVector(DScale.X, 1, 4)));
			HISMList[0]->AddInstance(FTransform(FRotator(0.0), FVector(Scale.X * signX, 0, 250), FVector(1, DScale.Y, 4)));
			HISMList[0]->AddInstance(FTransform(FRotator(0.0), FVector(-Scale.X * signX, 0, 250), FVector(1, DScale.Y, 4)));

			HISMList[1]->AddInstance(FTransform(FRotator(0.0), FVector(-Scale.X * signX, 0, 0), FVector(1, DScale.Y, 1)));
			HISMList[1]->AddInstance(FTransform(FRotator(0,180,0), FVector(Scale.X * signX, 0, 0), FVector(1, DScale.Y, 1)));
			HISMList[1]->AddInstance(FTransform(FRotator(0, 90, 0), FVector(0, Scale.Y * signY, 0), FVector(1, DScale.X, 1)));
			HISMList[1]->AddInstance(FTransform(FRotator(0, 270, 0), FVector(0, -Scale.Y * signY, 0), FVector(1, DScale.X, 1)));

			HISMList[1]->AddInstance(FTransform(FRotator(180,180,0), FVector(-Scale.X * signX, 0, 500), FVector(1, DScale.Y, 1)));
			HISMList[1]->AddInstance(FTransform(FRotator(180, 0, 0), FVector(Scale.X * signX, 0, 500), FVector(1, DScale.Y, 1)));
			HISMList[1]->AddInstance(FTransform(FRotator(180, 270, 0), FVector(0, Scale.Y * signY, 500), FVector(1, DScale.X, 1)));
			HISMList[1]->AddInstance(FTransform(FRotator(180, 90, 0), FVector(0, -Scale.Y * signY, 500), FVector(1, DScale.X, 1)));

			HISMList[1]->AddInstance(FTransform(FRotator(0, 180, 90), FVector(Scale.X * signX, Scale.Y * signY, 250), FVector(1, 4, 1)));
			HISMList[1]->AddInstance(FTransform(FRotator(0, 90, 90), FVector(-Scale.X * signX, Scale.Y * signY, 250), FVector(1, 4, 1)));
			HISMList[1]->AddInstance(FTransform(FRotator(0, 270, 90), FVector(Scale.X * signX, -Scale.Y * signY, 250), FVector(1, 4, 1)));
			HISMList[1]->AddInstance(FTransform(FRotator(0, 0, 90), FVector(-Scale.X * signX, -Scale.Y * signY, 250), FVector(1, 4, 1)));

			HISMList[2]->AddInstance(FTransform(FRotator(180,0,0), FVector(Scale.X * signX, Scale.Y * signY, 500), FVector(1)));
			HISMList[2]->AddInstance(FTransform(FRotator(180,90,0), FVector(Scale.X * signX, -Scale.Y * signY, 500), FVector(1)));
			HISMList[2]->AddInstance(FTransform(FRotator(180,270,0), FVector(-Scale.X * signX, Scale.Y * signY, 500), FVector(1)));
			HISMList[2]->AddInstance(FTransform(FRotator(180,180,0), FVector(-Scale.X * signX, -Scale.Y * signY, 500), FVector(1)));

			HISMList[2]->AddInstance(FTransform(FRotator(0, 90, 0), FVector(Scale.X * signX, Scale.Y * signY, 0), FVector(1)));
			HISMList[2]->AddInstance(FTransform(FRotator(0, 180, 0), FVector(Scale.X * signX, -Scale.Y * signY, 0), FVector(1)));
			HISMList[2]->AddInstance(FTransform(FRotator(0, 0, 0), FVector(-Scale.X * signX, Scale.Y * signY, 0), FVector(1)));
			HISMList[2]->AddInstance(FTransform(FRotator(0, 270, 0), FVector(-Scale.X * signX, -Scale.Y * signY, 0), FVector(1)));

			break;
		case cylinder:
			
			break;
	}
}