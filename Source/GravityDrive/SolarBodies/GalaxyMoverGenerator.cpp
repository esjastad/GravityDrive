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
	for (int i = 0; i < HISMList.Num(); ++i)
	{
		HISMList[i]->SetStaticMesh(StarMesh);
		HISMList[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HISMList[i]->SetMaterial(0, UMaterialInstanceDynamic::Create(OStarMat,this));
	}
	GenerateCenter();
}

void AGalaxyMoverGenerator::GenerateCenter()
{
	//Star Generation TESTING
	for (float i = -180; i < 180; i += 0.5)
	{	
		if (abs(i) > 1)
		{
			float Length = 50;
			float XVal = i * Length;
			
			float Angle = GPhi(Length, 0.5, XVal);
			float Scalar = UKismetMathLibrary::Log(abs(XVal), 10) * 2;
			
			
			FVector Direction((i * Scalar * SOLARSYSTEMAVERAGE), 0, 0);
			
			FVector SpawnLoc = UKismetMathLibrary::RotateAngleAxis(Direction, Angle, FVector(0, 0, 1));
			//for (int j = 0; j < (1 /*- (abs((i/10) * 0.5))*/); ++j)
			for (int j = 0; j < 1; ++j)
			{
				FVector Rando = UKismetMathLibrary::RandomUnitVectorFromStream(GalaxyGenStream) * UKismetMathLibrary::RandomFloatInRangeFromStream(SOLARSYSTEMAVERAGE ,SOLARSYSTEMAVERAGE * 100, GalaxyGenStream);
				//if (!(UKismetMathLibrary::IsPointInBox(SpawnLoc + Rando, FVector(0, 0, 500), FVector(500, 500, 50000)))) 
				{
					FTransform SpawnTForm(FRotator(0), SpawnLoc + Rando, FVector(2000));
					int index = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, HISMList.Num() - 1, GalaxyGenStream);
					HISMList[index]->AddInstance(SpawnTForm);
				}
				
			}
			
		}
	}

	//Get random Scale for supermassive black hole
	/*float CenterScale = UKismetMathLibrary::RandomFloatInRangeFromStream(SUPERBLACKHOLEMIN, SUPERBLACKHOLEMAX, GalaxyGenStream);
	ASolarBody * CenterBody = GetWorld()->SpawnActor<ASolarBody>(SolarBodyBP, FTransform(FRotator(0.0), FVector(0.0), FVector(1.0)), FActorSpawnParameters());
	CenterBody->SetBodyData(SolarBodyType::SBSun, CenterScale);

	FVector Offset = FVector(CenterScale * STARRADIUS, 0, 0);
	FVector EventHorizon = FVector(CenterScale * EVENTHORIZON, 0, 0);
	FVector SolarSystemSpace = FVector((SOLARSYSTEMAVERAGE * 2), 0, 0);
	CurrentLocation += Offset + EventHorizon + SolarSystemSpace;

	CenterBody->AttachToComponent(Mover, FAttachmentTransformRules::KeepWorldTransform);
	Mover->AddLocalOffset(CurrentLocation);

	GenerateStars();*/
}

void AGalaxyMoverGenerator::GenerateStars()
{

}


/**
 * Inputs:: R = Radius, Phi = Degree of turn from 0 to 1, Val = Input such as XGraph Location
 * Outputs = Degree of rotation for vector(Val, 0, 0) to match spiral galaxy formation
 */
float AGalaxyMoverGenerator::GPhi(float R, float Phi, float Val)
{
	float first = Val / Phi;
	float second = UKismetMathLibrary::Log(abs(first), 10);
	float third = UKismetMathLibrary::Tan(Phi);
	float fourth = second * third * Phi;
	float fifth = 1 - fourth;
	float sixth = R / fifth;
	return sixth;
}