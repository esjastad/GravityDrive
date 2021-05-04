// ©, 2021, Erik Jastad. All Rights Reserved


#include "GalaxyMoverGenerator.h"
#include "Kismet/KismetMathLibrary.h"
#include "../ShipFlight/QuatRotations.h"

// Sets default values
AGalaxyMoverGenerator::AGalaxyMoverGenerator() : AGalaxyMover()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StarLight = nullptr;
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
	MPC = GetWorld()->GetParameterCollectionInstance(MPCAsset);
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
	// Loop through the Star HISMS
	for (int i = 0; i < HISMList.Num(); ++i)
	{
		// If the default vector is not used then we must be spawning stars so use a larger radius
		int Scalar = (Loc == FVector(0)) ? 20 : 41;
		
		// Get Star instances that are close enough to the ship
		IntArray = HISMList[i]->GetInstancesOverlappingSphere(Loc, SOLARSYSTEMAVERAGE * Scalar);

		// If Star instance were found assign which index was hit and end the loop
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
	// Get HISM hit results on wether or not a star was found.
	int HitIndex = StarCheck(&HISMHit);

	//If we have a Star currently spawned then check if its distance is far anough away to spawn the HISM back in its place
	if (SpawnedStar)
	{
		// Garbage used as a required fillable vector for an Engine function call
		FVector Garbage;
		// Vector Length Distance from Ship location
		float Distance;
		(SpawnedStar->GetActorLocation()).ToDirectionAndLength(Garbage, Distance);
		// If moved enough
		if (Distance > SOLARSYSTEMAVERAGE * 20.01)
		{
			//World Transform for HISM to add
			FTransform HISMXForm = SpawnedStar->GetTransform();
			// Need to adjust the Transform location to account for the World offset of the Mover vs the Actor(Star) current location
			FVector StarLoc = HISMXForm.GetLocation();
			FVector MoverLoc = Mover->GetComponentLocation();
			FVector DifferenceLoc = StarLoc - MoverLoc;
			HISMXForm.SetLocation(DifferenceLoc);

			// Spawn the HISM and Destroy the Star Actor
			HISMList[SpawnedStar->HISMIndex]->AddInstance(HISMXForm);
			SpawnedStar->Destroy();
			SpawnedStar = nullptr;
		}
	}

	// If we hit a star
	if (HISMHit >= 0)
	{
		// Debugging, if we are already pointing at an existing star and we hit a new one.... print to screen
		if (SpawnedStar)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Hit a new index but we still referencing a star...."));
		}

		// Get the hit stars world transform
		FTransform HISMStarXForm;
		HISMList[HISMHit]->GetInstanceTransform(HitIndex, HISMStarXForm, true);
		
		// Spawn in a star actor and set its data
		SpawnedStar = GetWorld()->SpawnActor<ASolarBody>(SolarBodyBP, FTransform(HISMStarXForm.GetRotation(), HISMStarXForm.GetLocation(), FVector(1.0)), FActorSpawnParameters());
		SpawnedStar->AttachToComponent(Mover, FAttachmentTransformRules::KeepWorldTransform);
		SpawnedStar->SetBodyData(SolarBodyType::SBSun, HISMStarXForm.GetScale3D().X, HISMHit);

		// Remove the HISM star
		HISMList[HISMHit]->RemoveInstance(HitIndex);
	}

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

void AGalaxyMoverGenerator::UpdateLight()
{	
	if (SpawnedStar)
	{		
		FVector CurrLoc = -SpawnedStar->GetActorLocation();
		
		float Intensity;
		FVector Direction = (FVector(0,0,0));
		CurrLoc.ToDirectionAndLength(Direction,Intensity);
		Intensity = (SOLARSYSTEMAVERAGE * 19) / Intensity;
		Intensity *= 0.5;
		Intensity = Intensity * Intensity;
		Intensity = (Intensity > 0.3) ? Intensity : 0;
		
		FRotator Temp = UKismetMathLibrary::FindLookAtRotation(StarLight->GetForwardVector(), CurrLoc);
		//StarLight->AddLocalRotation(FRotator(0,1,0));
		//FRotator Temp = StarLight->GetComponentRotation() + FRotator(0, 1, 0);
		StarLight->SetWorldRotation(Temp);
		StarLight->SetIntensity(Intensity);
		MPC->SetVectorParameterValue("Loc", -CurrLoc);

		//FRotator Temp = FRotator(Direction.Z * 180, Direction.X * 180 - Direction.Y * 180, 0);
		//FQuat Rot = UKismetMathLibrary::Quat_MakeFromEuler(Direction * -1);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Distance %f"), Intensity));
		//UKismetMathLibrary::Sin();
		//UKismetMathLibrary::Cos();
		//FRotator Temp = UKismetMathLibrary::MakeRotFromY(Direction);
		//FRotator Temp = UKismetMathLibrary::MakeRotFromZ(Direction);
		
		//FRotator TEST(Temp.Yaw, Temp.Pitch, Temp.Roll);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("X Y %f %f"), Direction.X, Direction.Y));
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Loc %f %f %f"), Temp.Pitch, Temp.Yaw, Temp.Roll));
		
		//Direction = Direction * -1;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Loc %f %f %f"), Direction.X, Direction.Y, Direction.Z));
		/*CurrLoc.Normalize();
		//Distance = (SOLARSYSTEMAVERAGE * 25) - Distance;
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Distance %f"), Distance));
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Loc %f %f %f"), CurrLoc.X, CurrLoc.Y, CurrLoc.Z));
		FRotator Temp = UKismetMathLibrary::FindLookAtRotation(SpawnedStar->GetActorLocation(), FVector(0));
		Temp = UKismetMathLibrary::MakeRotFromX(CurrLoc);
		StarLight->SetWorldRotation(Temp);*/
		//StarLight->SetIntensity(Distance);
	}
}