// ©, 2021, Erik Jastad. All Rights Reserved


#include "GalaxyMoverGenerator.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGalaxyMoverGenerator::AGalaxyMoverGenerator() : AGalaxyMover()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UHierarchicalInstancedStaticMeshComponent * NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("OSTAR");
	NewHISM->SetupAttachment(Mover);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("BSTAR");
	NewHISM->SetupAttachment(Mover);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("ASTAR");
	NewHISM->SetupAttachment(Mover);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("FSTAR");
	NewHISM->SetupAttachment(Mover);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("GSTAR");
	NewHISM->SetupAttachment(Mover);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("KSTAR");
	NewHISM->SetupAttachment(Mover);
	HISMList.Add(NewHISM);

	NewHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("MSTAR");
	NewHISM->SetupAttachment(Mover);
	HISMList.Add(NewHISM);
}

// Called when the game starts or when spawned
void AGalaxyMoverGenerator::BeginPlay()
{
	Super::BeginPlay();

	GenerateCenter();
}

void AGalaxyMoverGenerator::GenerateCenter()
{
	//Get random Scale for supermassive black hole
	float CenterScale = UKismetMathLibrary::RandomFloatInRangeFromStream(SUPERBLACKHOLEMIN, SUPERBLACKHOLEMAX, GalaxyGenStream);
	ASolarBody * CenterBody = GetWorld()->SpawnActor<ASolarBody>(SolarBodyBP, FTransform(FRotator(0.0), FVector(0.0), FVector(1.0)), FActorSpawnParameters());
	CenterBody->SetBodyData(SolarBodyType::SBSun, CenterScale);

	FVector Offset = FVector(CenterScale * STARRADIUS, 0, 0);
	FVector EventHorizon = FVector(CenterScale * EVENTHORIZON, 0, 0);
	FVector SolarSystemSpace = FVector((SOLARSYSTEMAVERAGE * 2), 0, 0);
	CurrentLocation += Offset + EventHorizon + SolarSystemSpace;

	CenterBody->AttachToComponent(Mover, FAttachmentTransformRules::KeepWorldTransform);
	Mover->AddLocalOffset(CurrentLocation);

	GenerateStars();
}

void AGalaxyMoverGenerator::GenerateStars()
{
	//Calc Star Scale
	FVector StarOffset; 
	FVector SpawnLoc;
	Mover->AddLocalOffset(FVector(SOLARSYSTEMAVERAGE,0,0));

	for(int i = 0; i < 360; ++i)
	{
		SpawnLoc = UKismetMathLibrary::RotateAngleAxis(CurrentLocation + (UKismetMathLibrary::RandomUnitVectorFromStream(GalaxyGenStream) * SOLARSYSTEMAVERAGE * 2), i, FVector(0, 0, 1));
		for (int j = 0; j < 10; ++j)
		{
			float Scale = UKismetMathLibrary::RandomFloatInRangeFromStream(STARSCALEMIN, 500, GalaxyGenStream);
			StarOffset = UKismetMathLibrary::RotateAngleAxis(FVector(SOLARSYSTEMAVERAGE * 4, 0, 0), i, FVector(0, 0, 1));
			FTransform SpawnTForm = FTransform(FRotator(0),SpawnLoc + (j * StarOffset),FVector(Scale));
			int index = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, HISMList.Num() - 1, GalaxyGenStream);
			HISMList[index]->AddInstance(SpawnTForm);
			
		}
	}
	
	
	//Calc star type

	//Add star to correct hism (based on star type chose) with scale

	//Save Location Data?


	//ASolarBody * CenterBody = GetWorld()->SpawnActor<ASolarBody>(SolarBodyBP, FTransform(FRotator(0.0), FVector(0.0), FVector(1.0)), FActorSpawnParameters());
	//CenterBody->SetBodyData(SolarBodyType::SBSun, CenterScale);

	/*FVector Offset = FVector(CenterScale * STARRADIUS, 0, 0);
	FVector Atmosphere = FVector(CenterScale * EVENTHORIZON, 0, 0);
	CurrentLocation += Offset + Atmosphere;*/

	//CenterBody->AttachToComponent(Mover, FAttachmentTransformRules::KeepWorldTransform);
	//Mover->AddLocalOffset(CurrentLocation);
}