// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	float SetBodyData(SolarBodyType NewType, float Scale);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Body;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Sun;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* GasGiant;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* TerrestrialPlanet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Moon;

	SolarBodyType BodyType;
};
