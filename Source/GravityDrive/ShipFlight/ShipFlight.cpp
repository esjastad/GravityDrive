// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipFlight.h"
#include "../Characters/GDBaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AShipFlight::AShipFlight()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>("MyRoot");
	RootComponent = Scene;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bDoCollisionTest = false;
	SpringArm->TargetArmLength = ARMDEFAULTLENGTH * 5;
}

// Called when the game starts or when spawned
void AShipFlight::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	Camera->AddLocalOffset(FVector(0, 0, 1500));
	VoxelShip->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	MPC = GetWorld()->GetParameterCollectionInstance(MPCAsset);
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShipManager::StaticClass(), FoundActors);
	BAShipManager = FoundActors[0];
	/*TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARecastNavMesh::StaticClass(), FoundActors);
	RNavMesh = Cast<ARecastNavMesh>(FoundActors[0]);
	/*FoundActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANavMeshBoundsVolume::StaticClass(), FoundActors);
	NavMeshBV = Cast<ANavMeshBoundsVolume>(FoundActors[0]);
	
	RNavMesh->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	NavMeshBV->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);*/
}

// Called every frame
void AShipFlight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PController)
	{
		//Rotate Ship
		FRotator Current = GetActorRotation();
		FRotator Target = PController->GetControlRotation();
		SetActorRotation(FMath::RInterpTo(Current, Target, DeltaTime, 1));
		
		//Update Material Params
		MPC->SetVectorParameterValue(FName("Normal"), FLinearColor(GetActorUpVector()));

		//Get Offset of ship manager to update MPC with
		FVector Result = GetTransform().InverseTransformPosition(BAShipManager->GetActorLocation()).GridSnap(XYSIZE) + FVector(0,0,ZFLOORHALF);
		FVector Adjusted = UKismetMathLibrary::Quat_RotateVector(FQuat(GetActorRotation()), Result);
		MPC->SetVectorParameterValue(FName("Position"), Adjusted);
	}
}

// Called to bind functionality to input
void AShipFlight::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Called when possessed
void AShipFlight::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PController = Cast<APlayerController>(NewController);
	// If possessed by player controller
	if (PController)
	{
		SetActorTickEnabled(true);
	}
}

// Called when unpossessed
void AShipFlight::UnPossessed()
{
	PController = nullptr;
	SetActorTickEnabled(false);
}
