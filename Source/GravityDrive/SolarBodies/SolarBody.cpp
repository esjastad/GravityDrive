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
	yaw += (BodyType == SBSun) ? 0.000001 : 0.0000416;	// Add to the current yaw rotation (Makes orbiting bodies seem like they are orbiting)
	
	SetActorRelativeRotation(FRotator(0, yaw, 0));	// Set the rotation to the new yaw
}

/* 
 * Should be called when spawned
 * This function is used to update the Mesh, Type, Scale, 
 * the index passed in is only important if this is a SUN because it indicates which HISM index in the GalaxyMoverGenerator to add it back to it was when despawning this actor
 */
float ASolarBody::SetBodyData(SolarBodyType NewType, float Scale, int Index)
{
	HISMIndex = Index;		
	BodyType = NewType;

	// Set ProceduralStream Value, needs to be adjusted so it is set based on this Stars/Planets World location relative to its parent
	FVector Loc = GetActorLocation();
	float Length;
	Loc.ToDirectionAndLength(Loc, Length);	
	SolarGenStream = Length;

	// Depending on our type set the Mesh and Material used
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
	case SBBlackHole:
		Body->SetStaticMesh(Sun);
		Body->CastShadow = false;
		Body->SetMaterial(0, UMaterialInstanceDynamic::Create(BlackHoleMat, this));
		Atmosphere->DestroyComponent();
	}

	Body->SetWorldScale3D(FVector(Scale));		// Set our Bodies Scale
	
	return 1;		// value Not used at the moment
}

// Spawn Planets 
void ASolarBody::SpawnPlanetsForStar()
{
	int limit = UKismetMathLibrary::RandomIntegerInRangeFromStream(5, 13, SolarGenStream); // Limit on for loop
	for (int i = 0; i < limit; ++i)
	{
		// Spawn and attach orbiting body
		ASolarBody * NewBody = GetWorld()->SpawnActor<ASolarBody>(SolarBodyBP, FTransform(FRotator(0), FVector(0), FVector(1.0)), FActorSpawnParameters());
		NewBody->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		// Calculate/Pick planet type, Scale, Angle and Offset for where in orbit the orbiting body should start
		SolarBodyType PlanetType = (i > 2) ? SolarBodyType::SBTerrestrial : (UKismetMathLibrary::RandomIntegerInRangeFromStream(1, 2, SolarGenStream) == 1) ? SolarBodyType::SBGasGiant : SolarBodyType::SBTerrestrial;
		float Scale = (PlanetType == SolarBodyType::SBTerrestrial) ? UKismetMathLibrary::RandomFloatInRangeFromStream(PLANETMIN, PLANETMAX, SolarGenStream) : UKismetMathLibrary::RandomFloatInRangeFromStream(GASGIANTMIN, GASGIANTMAX, SolarGenStream);
		float Angle = UKismetMathLibrary::RandomFloatInRange(0, 360);
		FVector Offset = UKismetMathLibrary::RotateAngleAxis(FVector(SOLARSYSTEMAVERAGE * -0.001 * (i + 1), 0, 0), Angle, FVector(0,0,1));
		// Set Location and Data for new orbiting body and add a reference to the array
		NewBody->AddActorLocalOffset(Offset);
		NewBody->SetBodyData(PlanetType, Scale, -1);
		OrbitingBodies.Add(NewBody);
	}
}

// Spawn Moons
void ASolarBody::SpawnMoonsForPlanet(SolarBodyType ForType)
{

}

//When we are destroyed cycle through our planets/moons and destroy them..
void ASolarBody::Destroyed()
{
	Super::Destroyed();
	for (auto &elem : OrbitingBodies)
	{
		elem->Destroy();
	}
}