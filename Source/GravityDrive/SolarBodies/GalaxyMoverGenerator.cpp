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
	for (float i = -180; i < 180; i += 0.5)
	{	
		if (abs(i) > 1)
		{
			float Length = 50;
			float XVal = i * Length;
			//float YVal = GFit(Length, 0.5, 4, XVal);
			float Angle = GPhi(Length, 0.5, XVal);
			float Scalar = UKismetMathLibrary::Log(abs(XVal), 10) * 2;
			
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("GFit %f"), Scalar));
			//FVector Direction((XVal * Scalar), 0, 0);
			FVector Direction((i * Scalar * SOLARSYSTEMAVERAGE), 0, 0);
			//GFit(100, 4, 0.5, i) * 100
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("GFit %f"), YVal));
			//FVector SpawnLoc(XVal, YVal, 500);
			FVector SpawnLoc = UKismetMathLibrary::RotateAngleAxis(Direction, Angle, FVector(0, 0, 1));
			for (int j = 0; j < (10 /*- (abs((i/10) * 0.5))*/); ++j)
			{
				FVector Rando = UKismetMathLibrary::RandomUnitVectorFromStream(GalaxyGenStream) * UKismetMathLibrary::RandomFloatInRangeFromStream(SOLARSYSTEMAVERAGE ,SOLARSYSTEMAVERAGE * 100, GalaxyGenStream);
				//if (!(UKismetMathLibrary::IsPointInBox(SpawnLoc + Rando, FVector(0, 0, 500), FVector(500, 500, 50000)))) 
				{
					FTransform SpawnTForm(FRotator(0), SpawnLoc + Rando, FVector(500));
					//FTransform SpawnTForm(FRotator(0), SpawnLoc, FVector(500));
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
	//Calc Star Scale
	/*FVector StarOffset; 
	FVector SpawnLoc;

	//Move futher away from spawn loc possible
	Mover->AddLocalOffset(FVector(SOLARSYSTEMAVERAGE,0,0));

	for(int i = 1; i < 361; ++i)
	{
		//SpawnLoc = UKismetMathLibrary::RotateAngleAxis(CurrentLocation + (UKismetMathLibrary::RandomUnitVectorFromStream(GalaxyGenStream) * SOLARSYSTEMAVERAGE * 2), i * 2, FVector(0, 0, 1));
		float test = GFit(1, 1, 4, (i * 0.01));
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("GFit %f"), test));

		SpawnLoc = UKismetMathLibrary::RotateAngleAxis(CurrentLocation * i, i*8, FVector(0, 0, 1));

		//for (int j = 0; j < 20; ++j)
		{
			//float Scale = UKismetMathLibrary::RandomFloatInRangeFromStream(STARSCALEMIN, 4100, GalaxyGenStream);
			float Scale = UKismetMathLibrary::RandomFloatInRangeFromStream(500, 500, GalaxyGenStream);
			StarOffset = UKismetMathLibrary::RotateAngleAxis(FVector(SOLARSYSTEMAVERAGE * 40, 0, 0), i * 2, FVector(0, 0, 1));
			//FTransform SpawnTForm = FTransform(FRotator(0),SpawnLoc + (j * StarOffset),FVector(Scale));
			FTransform SpawnTForm = FTransform(FRotator(0), SpawnLoc, FVector(Scale));
			int index = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, HISMList.Num() - 1, GalaxyGenStream);
			HISMList[index]->AddInstance(SpawnTForm);
			SpawnTForm = FTransform(FRotator(0), -SpawnLoc, FVector(Scale));
			HISMList[index]->AddInstance(SpawnTForm);
		}
	}*/
	
	
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

/**
 * A is a scaler
 * B and N adjust the pitch of the Spiral
 */

float AGalaxyMoverGenerator::GFit(float A, float B, float N, float phi)
{
	float first = 2 * N;
	float second = phi / first;
	float third = UKismetMathLibrary::Tan(second);
	
	float fourth = B * third;
	float fifth = UKismetMathLibrary::Log(abs(fourth),10);
	/*if (isnan(fifth))
	{
		fifth = UKismetMathLibrary::Log(abs(fourth), 10);
		
	}*/
	float sixth = A / fifth;

	/*float TanResult = UKismetMathLibrary::Tan(phi / (2 * N));
	float LogResult = UKismetMathLibrary::Log(B * TanResult,10);
	float MathResult = A / LogResult;
	return A / LogResult;*/

	//float allOne = (A / UKismetMathLibrary::Log((B * UKismetMathLibrary::Tan(phi / (2 * N))), 10));

	return sixth;
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