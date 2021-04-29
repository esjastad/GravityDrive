// ©, 2021, Erik Jastad. All Rights Reserved


#include "GDBaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "../ShipFlight/ShipFlight.h"

// Default Constructor, Sets default values for class
AGDBaseCharacter::AGDBaseCharacter(const FObjectInitializer& ObjectInitializer) : AGravityCharacter(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	// Setup arm and camera
	CamSpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	OTSCamera = CreateDefaultSubobject<UCameraComponent>("Camera");
	
	// Setup arm and camera heirarchy
	CamSpringArm->SetupAttachment(RootComponent);
	OTSCamera->SetupAttachment(CamSpringArm);
	GravityDir = FVector(0);
}

// Update Gravity Orientation
void AGDBaseCharacter::GravityUpdate(FVector GravityCenter)
{
	GravityDir = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), GravityCenter);
}

// Called when the game starts or when spawned
void AGDBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShipFlight::StaticClass(), FoundActors);
	GravityActor = FoundActors[0];

}

// Called every frame
void AGDBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GravityUpdate(GravityActor->GetActorUpVector() * -50000);
	/*if (PC)
	{
		float test = PC->InputComponent->GetAxisKeyValue("LookRight");
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("value %f"), test));
	}*/
	
	
	//CamSpringArm->AddRelativeRotation(FRotator(0, 0, 0));
}

// Called to bind functionality to input
void AGDBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Bind defined project inputs to respective functions
	InputComponent->BindAxis("MoveForward", this, &AGDBaseCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AGDBaseCharacter::MoveRight);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AGDBaseCharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &AGDBaseCharacter::StopJumping);
}

// Called when possessed by a new controller
void AGDBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PC = Cast<APlayerController>(NewController);
}

void AGDBaseCharacter::UnPossessed()
{
	if (PC)
	{
		this->SpawnDefaultController();
	}
}

void AGDBaseCharacter::TurnRight(float Value)
{
	AddActorLocalRotation(FRotator(0,Value,0));
}

void AGDBaseCharacter::LookUp(float Value)
{
	CamSpringArm->AddRelativeRotation((abs(CamSpringArm->GetRelativeRotation().Pitch + Value) >= 90.0) ? FRotator(0, 0, 0) : FRotator(Value, 0, 0));
	//CamSpringArm->AddRelativeRotation(FRotator(Value, 0, 0));
}

// Controls forward and backward movement, input set in SetupPlayerInputcomponent()
void AGDBaseCharacter::MoveForward(float Value)
{
	//If possessed and the input is not 0
	if ((Controller) && Value != 0.0f) {
		AddMovementInput(GetActorForwardVector(), Value);
		
	}
}

// Controls right and left movement, input set in SetupPlayerInputcomponent()
void AGDBaseCharacter::MoveRight(float Value)
{
	//If possessed and the input is not 0
	if ((Controller) && Value != 0.0f) {
		AddMovementInput(GetActorRightVector(), Value);
	}
}


