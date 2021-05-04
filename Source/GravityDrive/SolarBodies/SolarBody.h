// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SolarBodyDefinitions.h"
#include "SolarBody.generated.h"

UCLASS()
class GRAVITYDRIVE_API ASolarBody : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASolarBody();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	void SpawnPlanetsForStar();
	void SpawnMoonsForPlanet();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	float SetBodyData(SolarBodyType NewType, float Scale, int Index);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Body;
	UStaticMeshComponent* Atmosphere;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Sun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* GasGiant;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* TerrestrialPlanet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Moon;

	SolarBodyType BodyType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMaterialInstance*> StarMatList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASolarBody> SolarBodyBP;

	TArray<USpringArmComponent*> OrbitingArms;
	TArray<ASolarBody*> OrbitingBodies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance * AtmosphereMat;

	// Hit HISM, used for STARS to select which material and recalled to use when spawning HISM back in world
	int HISMIndex;
	float yaw;
};
