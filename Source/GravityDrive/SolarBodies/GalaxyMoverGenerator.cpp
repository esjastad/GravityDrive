// ©, 2021, Erik Jastad. All Rights Reserved


#include "GalaxyMoverGenerator.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGalaxyMoverGenerator::AGalaxyMoverGenerator() : AGalaxyMover()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpawnedStar = nullptr;

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
		HISMList[i]->SetMaterial(0, UMaterialInstanceDynamic::Create(StarMatList[i],this));
	}
	GenerateCenter();
	
}

// Check for nearby stars
int AGalaxyMoverGenerator::StarCheck(int * HISMHit, FVector Loc)
{
	FJsonSerializableArrayInt IntArray;
	for (int i = 0; i < HISMList.Num(); ++i)
	{
		int Scalar = (Loc == FVector(0)) ? 20 : 41;
		//Trace for Ship Segment and call mouseover to display anchor points	
		IntArray = HISMList[i]->GetInstancesOverlappingSphere(Loc, SOLARSYSTEMAVERAGE * Scalar);
		if (IntArray.Num())
		{
			*HISMHit = i;
			break;
		}
	}
	return (IntArray.Num()) ? IntArray[0] : 0;
}

// Swap actor/instance for actor/instance
void AGalaxyMoverGenerator::InstanceSwap()
{
	int HISMHit = -1;
	int HitIndex = StarCheck(&HISMHit);

	/*if (SpawnedStar)
	{
		FVector Garbage;
		float Distance;
		(CurrentLocation - SpawnedStar->GetActorLocation()).ToDirectionAndLength(Garbage, Distance);
		// If moved enough
		if (Distance > SOLARSYSTEMAVERAGE * 21)
		{
			FTransform HISMXForm = SpawnedStar->GetTransform();
			//HISMXForm.SetScale3D(SpawnedStar->Body->GetComponentScale());
			HISMList[SpawnedStar->HISMIndex]->AddInstance(HISMXForm);
			SpawnedStar->Destroy();
			SpawnedStar = nullptr;
		}
	}*/

	if (HISMHit >= 0)
	{
		if (SpawnedStar)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Hit a new index but we still referencing a star...."));
		}

		FTransform HISMStarXForm;
		HISMList[HISMHit]->GetInstanceTransform(HitIndex, HISMStarXForm, true);
		
		SpawnedStar = GetWorld()->SpawnActor<ASolarBody>(SolarBodyBP, FTransform(HISMStarXForm.GetRotation(), HISMStarXForm.GetLocation(), FVector(1.0)), FActorSpawnParameters());
		SpawnedStar->AttachToComponent(Mover, FAttachmentTransformRules::KeepWorldTransform);
		SpawnedStar->SetBodyData(SolarBodyType::SBSun, HISMStarXForm.GetScale3D().X, HISMHit);
		HISMList[HISMHit]->RemoveInstance(HitIndex);
	}

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Hit Index = %d"), HitIndex));
}

void AGalaxyMoverGenerator::GenerateCenter()
{
	//Get random Scale for supermassive black hole
	float CenterScale = UKismetMathLibrary::RandomFloatInRangeFromStream(SUPERBLACKHOLEMIN, SUPERBLACKHOLEMAX, GalaxyGenStream);
	ASolarBody * CenterBody = GetWorld()->SpawnActor<ASolarBody>(SolarBodyBP, FTransform(FRotator(0.0), FVector(0.0), FVector(1.0)), FActorSpawnParameters());
	CenterBody->SetBodyData(SolarBodyType::SBSun, CenterScale, -1);

	FVector Offset = FVector(CenterScale * STARRADIUS, 0, 0);
	FVector EventHorizon = FVector(CenterScale * EVENTHORIZON, 0, 0);
	FVector SolarSystemSpace = FVector((SOLARSYSTEMAVERAGE * 2), 0, 0);
	
	CurrentLocation += Offset + EventHorizon + SolarSystemSpace;
	CenterBody->AttachToComponent(Mover, FAttachmentTransformRules::KeepWorldTransform);
	Mover->AddLocalOffset(CurrentLocation);
	LastStarCheckLocation = CurrentLocation;

	GenerateStars();
}

int AGalaxyMoverGenerator::MakeWeightedStarSpawnIndex()
{
	// 0 to 100%
	float rando = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, 100, GalaxyGenStream);
	int result;

	if (rando > 6.5)
	{
		if (rando > 15)
		{
			result = 6;
		}
		else
		{
			if (rando > 10)
			{
				result = 5;
			}
			else
			{
				result = 4;
			}
		}
	}
	else
	{
		if (rando > 2.5)
		{
			if (rando > 5)
			{
				result = 3;
			}
			else
			{
				result = 2;
			}
		}
		else
		{
			if (rando > 1)
			{
				result = 1;
			}
			else
			{
				result = 0;
			}
		}
	}
	
	return result;
}

void AGalaxyMoverGenerator::GenerateStars()
{
	//Star Generation TESTING
	for (float i = -180; i < 180; i += 0.5)
	{
		//Leave central area empty
		if (abs(i) > 1)
		{
			float Length = 50;
			float XVal = i * Length;

			//Get the angle rotation for a spiral galaxy
			float Angle = GPhi(Length, 0.5, XVal);
			//Gradual additional distance scaling 
			float Scalar = UKismetMathLibrary::Log(abs(XVal), 10) * 2;

			//Vector to rotate
			FVector Direction((i * Scalar * SOLARSYSTEMAVERAGE), 0, 0);
			//Rotated vector aka SPAWNLOC
			FVector SpawnLoc = UKismetMathLibrary::RotateAngleAxis(Direction, Angle, FVector(0, 0, 1));
			//for (int j = 0; j < (1 /*- (abs((i/10) * 0.5))*/); ++j)
			//Spawn planets, thin out the number spawned as you get further out in spiral

			int limit = 50 - (abs(i / 4));
			for (int j = 0; j < limit ; ++j)
			{
				//Additional random offsets
				Scalar = SOLARSYSTEMAVERAGE * (j + 1);
				FVector Rando = UKismetMathLibrary::RandomUnitVectorFromStream(GalaxyGenStream) * UKismetMathLibrary::RandomFloatInRangeFromStream(Scalar, Scalar * 300, GalaxyGenStream);

				//Keep Center clear
				//if (!(UKismetMathLibrary::IsPointInBox(SpawnLoc + Rando, FVector(0, 0, 500), FVector(SOLARSYSTEMAVERAGE * 20000))))
				{


					//Controls Star Color, should also control Scale?
					int index = MakeWeightedStarSpawnIndex();//UKismetMathLibrary::RandomIntegerInRangeFromStream(0, HISMList.Num() - 1, GalaxyGenStream);
					//Random scale in range
					float WScale = UKismetMathLibrary::RandomFloatInRangeFromStream(StarScalesList[index].X, StarScalesList[index].Y, GalaxyGenStream);
					FTransform SpawnTForm(FRotator(0), SpawnLoc + Rando, FVector(WScale));
					int Garbage = -1;
					StarCheck(&Garbage, SpawnLoc + Rando);
					if(Garbage == -1)
						HISMList[index]->AddInstance(SpawnTForm);
				}

			}

		}
	}
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