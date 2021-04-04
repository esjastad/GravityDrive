// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "ShipSegment.generated.h"

UCLASS()
class GRAVITYDRIVE_API AShipSegment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShipSegment();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* Area;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
