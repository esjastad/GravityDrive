// ©, 2021, Erik Jastad. All Rights Reserved


#include "ShipFlight.h"
#include "../Characters/GDBaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"

// **********************************************Move into a ShipFlgith definitions file****************************
#define DEFSPEED 500000000
#define COMBATSPEED 5000

// Sets default values
AShipFlight::AShipFlight()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>("MyRoot");	// Create component
	RootComponent = Scene;	// Assign Root

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");	// Create component
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	SpringArm->SetupAttachment(RootComponent);	// Attach to Root
	Camera->SetupAttachment(SpringArm);

	Speed = DEFSPEED;	//Set default max speed
	SolarSpeed = false;		// Are we at a star? this influences the max speed allowed
}

// Called when the game starts or when spawned
void AShipFlight::BeginPlay()
{
	Super::BeginPlay();
	// Default setup, turn off tick, offset camera, Attach Voxel ship to this actor, Get MPC and the ShipManager....,
	SetActorTickEnabled(false);
	Camera->AddLocalOffset(FVector(0, 0, 1000));
	VoxelShip->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	MPC = GetWorld()->GetParameterCollectionInstance(MPCAsset);
	
}

// Called every frame
void AShipFlight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PController)
	{
		//Rotate Ship by interpolating Current Quat vs Target Quat and Setting Quat Rotation
		FQuat Current = FQuat(GetActorRotation());	
		FQuat Target = FQuat(Camera->GetComponentQuat());
		FQuat Interp = FMath::QInterpTo(Current,Target,DeltaTime,0.5);
		QuatRotations::SetActorWorldRotationQuat(this,Interp);
		
		//Update Material Params in Collection Parameter, Controls orientation of how ship material appears transparent when in ship building mode
		MPC->SetVectorParameterValue(FName("Normal"), FLinearColor(GetActorUpVector()));

		//Get Offset of ship manager to update MPC with, Controls the Z axis transparency position for ships material when in ship building mode
		FVector Result = GetTransform().InverseTransformPosition(BAShipManager->GetActorLocation()).GridSnap(XYSIZE) + FVector(0,0,ZFLOORHALF);
		FVector Adjusted = UKismetMathLibrary::Quat_RotateVector(FQuat(GetActorRotation()), Result);
		MPC->SetVectorParameterValue(FName("Position"), Adjusted);
	}
}

// Called to bind functionality to input
void AShipFlight::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("RollClockwise", this, &AShipFlight::RollClockwise);
	InputComponent->BindAxis("MoveForward", this, &AShipFlight::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AShipFlight::MoveRight);
	InputComponent->BindAction("LShift", IE_Pressed, this, &AShipFlight::SpeedShift);
	InputComponent->BindAxis("LookRight", this, &AShipFlight::TurnRight);
	InputComponent->BindAxis("LookUp", this, &AShipFlight::LookUp);
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

// Left Shift pressed and released, adjust ship speed
void AShipFlight::SpeedShift()
{
	if (Speed == DEFSPEED)
		//Speed = (SolarSpeed) ? 10000000000 : 50000000000;
		Speed = 100000000000;
							   
	else
		Speed = DEFSPEED;
	
}

// Move Forward/Backward respective to value pos/neg
void AShipFlight::MoveForward(float Value)
{
	if (Value != 0)
		FlightControl->AddVelocity(VoxelShip->GetActorForwardVector() * -Value * Speed);
}

// Move Right/Left respective to value pos/neg
void AShipFlight::MoveRight(float Value)
{
	if(Value != 0)
		FlightControl->AddVelocity(VoxelShip->GetActorRightVector() * -Value * Speed);
}

// Roll Right/Left respective to value pos/neg
void AShipFlight::RollClockwise(float Value)
{
	QuatRotations::AddLocalRotationQuat(SpringArm, QuatRotations::EulerToQuaternion(FRotator(0, 0, Value)));
}

// Look Up/Down respective to value pos/neg
void AShipFlight::LookUp(float Value)
{
	QuatRotations::AddLocalRotationQuat(SpringArm, QuatRotations::EulerToQuaternion(FRotator(Value, 0, 0)));
}

// Look Right/Left respective to value pos/neg
void AShipFlight::TurnRight(float Value)
{
	QuatRotations::AddLocalRotationQuat(SpringArm, QuatRotations::EulerToQuaternion(FRotator(0, Value, 0)));
}
