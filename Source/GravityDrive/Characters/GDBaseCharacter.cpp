// ©, 2021, Erik Jastad. All Rights Reserved


#include "GDBaseCharacter.h"

// Default Constructor, Sets default values for class
AGDBaseCharacter::AGDBaseCharacter()
{
 	// Set this character to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	// Setup arm and camera
	CamSpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	OTSCamera = CreateDefaultSubobject<UCameraComponent>("Camera");
	
	// Setup arm and camera heirarchy
	CamSpringArm->SetupAttachment(RootComponent);
	OTSCamera->SetupAttachment(CamSpringArm);
}

// Called when the game starts or when spawned
void AGDBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGDBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
}

// Controls forward and backward movement, input set in SetupPlayerInputcomponent()
void AGDBaseCharacter::MoveForward(float Value)
{
	//If possessed and the input is not 0
	if ((Controller) && Value != 0.0f) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

// Controls right and left movement, input set in SetupPlayerInputcomponent()
void AGDBaseCharacter::MoveRight(float Value)
{
	//If possessed and the input is not 0
	if ((Controller) && Value != 0.0f) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}


