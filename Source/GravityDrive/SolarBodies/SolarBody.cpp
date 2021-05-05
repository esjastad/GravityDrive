// ©, 2021, Erik Jastad. All Rights Reserved


#include "SolarBody.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASolarBody::ASolarBody()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HISMIndex = -1;
	Body = CreateDefaultSubobject<UStaticMeshComponent>("Body");
	RootComponent = Body;
	
	Atmosphere = CreateDefaultSubobject<UStaticMeshComponent>("Atmosphere");
	Atmosphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Atmosphere->SetupAttachment(Body);
	Atmosphere->SetRelativeScale3D(FVector(1.037));
	
	yaw = 0;
}

// Called when the game starts or when spawned
void ASolarBody::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASolarBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	yaw += (BodyType == SBSun) ? 0.000001 : 0.0000416;
	/*if (BodyType == SBSun) 
	{
		yaw += 0.000001;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Yaw %f"), yaw));
	}
	else
	{
		yaw += 0.0000416
	}*/
	SetActorRelativeRotation(FRotator(0, yaw, 0));
		//AddActorLocalRotation(FRotator(0, 0.001, 0));

}

float ASolarBody::SetBodyData(SolarBodyType NewType, float Scale, int Index)
{
	HISMIndex = Index;
	BodyType = NewType;

	FVector Loc = GetActorLocation();
	float Length;
	Loc.ToDirectionAndLength(Loc, Length);
	//Length = int(Length) % 10000000;
	SolarGenStream = Length;

	switch (NewType)
	{
	case SBSun:
		Body->SetStaticMesh(Sun);
		Body->CastShadow = false;
		Atmosphere->DestroyComponent();
		if (Index >= 0)
		{
			Body->SetMaterial(0, UMaterialInstanceDynamic::Create(StarMatList[Index], this));
			SpawnPlanetsForStar();
		}
		break;
	case SBGasGiant:
		Body->SetStaticMesh(GasGiant);	
		Atmosphere->SetStaticMesh(GasGiant);
		Atmosphere->SetMaterial(0, UMaterialInstanceDynamic::Create(AtmosphereMat, this));
		Atmosphere->SetRelativeScale3D(FVector(1.0001));
		// Spawn Moons here
		break;
	case SBTerrestrial:
		Body->SetStaticMesh(TerrestrialPlanet);
		Atmosphere->SetStaticMesh(TerrestrialPlanet);
		Atmosphere->SetMaterial(0, UMaterialInstanceDynamic::Create(AtmosphereMat, this));
		// Spawn Moons here
		break;
	case SBMoon:
		Body->SetStaticMesh(Moon);
		break;
	}
	Body->SetWorldScale3D(FVector(Scale));
	
	return 1;
}

void ASolarBody::SpawnPlanetsForStar()
{
	int limit = UKismetMathLibrary::RandomIntegerInRangeFromStream(5, 13, SolarGenStream);
	for (int i = 0; i < limit; ++i)
	{
		/*FString PString = FString::Printf(TEXT("%d"), i);
		FName Name(*PString);
		
		USpringArmComponent * NewArm = NewObject<USpringArmComponent>(this, Name);

		NewArm->TargetArmLength = SOLARSYSTEMAVERAGE * 0.005;
		NewArm->SetupAttachment(Body);
		OrbitingArms.Add(NewArm);
		
		ASolarBody * NewBody = GetWorld()->SpawnActor<ASolarBody>(SolarBodyBP, FTransform(FRotator(0), FVector(0), FVector(1.0)), FActorSpawnParameters());
		NewBody->AttachToComponent(NewArm, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewBody->SetBodyData(SolarBodyType::SBGasGiant, 9999, -1);*/

		ASolarBody * NewBody = GetWorld()->SpawnActor<ASolarBody>(SolarBodyBP, FTransform(FRotator(0), FVector(0), FVector(1.0)), FActorSpawnParameters());
		NewBody->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		// Pick planet type
		SolarBodyType PlanetType = (i > 2) ? SolarBodyType::SBTerrestrial : (UKismetMathLibrary::RandomIntegerInRangeFromStream(1, 2, SolarGenStream) == 1) ? SolarBodyType::SBGasGiant : SolarBodyType::SBTerrestrial;
		float Scale = (PlanetType == SolarBodyType::SBTerrestrial) ? UKismetMathLibrary::RandomFloatInRangeFromStream(PLANETMIN, PLANETMAX, SolarGenStream) : UKismetMathLibrary::RandomFloatInRangeFromStream(GASGIANTMIN, GASGIANTMAX, SolarGenStream);
		float Angle = UKismetMathLibrary::RandomFloatInRange(0, 360);
		FVector Offset = UKismetMathLibrary::RotateAngleAxis(FVector(SOLARSYSTEMAVERAGE * -0.001 * (i + 1), 0, 0), Angle, FVector(0,0,1));
		NewBody->AddActorLocalOffset(Offset);
		NewBody->SetBodyData(PlanetType, Scale, -1);
		OrbitingBodies.Add(NewBody);
	}
	
	/*GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("SpringArm Count = %d"), OrbitingArms.Num()));
	ASolarBody * NewBody = GetWorld()->SpawnActor<ASolarBody>(SolarBodyBP, FTransform(FRotator(0), FVector(0), FVector(1.0)), FActorSpawnParameters());
	NewBody->AttachToComponent(OrbitingArms[0], FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	NewBody->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	NewBody->SetBodyData(SolarBodyType::SBGasGiant, 19999, -1);*/
	//OrbitingArms[0]
	
}

void ASolarBody::SpawnMoonsForPlanet(SolarBodyType ForType)
{

}

void ASolarBody::Destroyed()
{
	Super::Destroyed();
	for (auto &elem : OrbitingBodies)
	{
		elem->Destroy();
	}
}