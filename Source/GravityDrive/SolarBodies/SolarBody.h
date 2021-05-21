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
	virtual void Destroyed() override;		// Called When Destroyed
	void SpawnPlanetsForStar();				// Spawns the planets
	void SpawnMoonsForPlanet(SolarBodyType ForType);	//Maybe combine with Spawn planets??

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	float SetBodyData(SolarBodyType NewType, float Scale, int Index);	//Set our data, type, scale, index

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Body;
	UStaticMeshComponent* Atmosphere;

	// Static mesh references set ONLY in Blueprint of this class!  ****Maybe Redo as an Array*****
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Sun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* GasGiant;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* TerrestrialPlanet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Moon;



	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMaterialInstance*> StarMatList;		//Materials used, Array and references only filled/added in an engine Blueprint of this class!
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASolarBody> SolarBodyBP;		//Subclass reference assign in Blueprint of this class! Used to spawn in the created engine BP of this class
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance * AtmosphereMat;		//Atmosphere material reference assigned in Blueprint of this class!
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance * BlackHoleMat;		//BlackHole material reference assigned in Blueprint of this class!

	//TArray<USpringArmComponent*> OrbitingArms;
	TArray<ASolarBody*> OrbitingBodies;		//Array of orbiting bodies spawned in for this solar body
	SolarBodyType BodyType;		//Enum that identifies what type we are
	
	FRandomStream SolarGenStream;		// Procedural generation stream

	int HISMIndex;		// Hit HISM, used for STARS to select which material and recalled to use when spawning HISM back in world
	float yaw;		// Current Yaw Rotation
};
