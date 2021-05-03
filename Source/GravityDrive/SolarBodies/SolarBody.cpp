// ©, 2021, Erik Jastad. All Rights Reserved


#include "SolarBody.h"

// Sets default values
ASolarBody::ASolarBody()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	HISMIndex = -1;
	Body = CreateDefaultSubobject<UStaticMeshComponent>("Body");
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

}

float ASolarBody::SetBodyData(SolarBodyType NewType, float Scale, int Index)
{
	HISMIndex = Index;
	BodyType = NewType;

	switch (NewType)
	{
	case SBSun:
		Body->SetStaticMesh(Sun);
		if(Index >= 0)
			Body->SetMaterial(0, UMaterialInstanceDynamic::Create(StarMatList[Index], this));
		break;
	case SBGasGiant:
		Body->SetStaticMesh(GasGiant);
		break;
	case SBTerrestrial:
		Body->SetStaticMesh(TerrestrialPlanet);
		break;
	case SBMoon:
		Body->SetStaticMesh(Moon);
		break;
	}
	Body->SetWorldScale3D(FVector(Scale));
	
	return 1;
}
