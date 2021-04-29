// ©, 2021, Erik Jastad. All Rights Reserved


#include "QuatRotations.h"

// Formula to convert a Euler angle in degrees to a quaternion rotation
FQuat QuatRotations::EulerToQuaternion(FRotator CurrentRotation)
{
	FQuat q;                                            // Declare output quaternion
	float yaw = CurrentRotation.Yaw * PI / 180;        // Convert degrees to radians 
	float roll = CurrentRotation.Roll * PI / 180;
	float pitch = CurrentRotation.Pitch * PI / 180;

	double cy = cos(yaw * 0.5);
	double sy = sin(yaw * 0.5);
	double cr = cos(roll * 0.5);
	double sr = sin(roll * 0.5);
	double cp = cos(pitch * 0.5);
	double sp = sin(pitch * 0.5);

	q.W = cy * cr * cp + sy * sr * sp;
	q.X = cy * sr * cp - sy * cr * sp;
	q.Y = cy * cr * sp + sy * sr * cp;
	q.Z = sy * cr * cp - cy * sr * sp;

	return q;                                           // Return the quaternion of the input Euler rotation
}

// Set the scene component's world rotation to the input quaternion
void QuatRotations::SetWorldRotationQuat(USceneComponent* SceneComponent, const FQuat& DesiredRotation)
{
	if (SceneComponent)
	{
		SceneComponent->SetWorldRotation(DesiredRotation);
	}
}

// Set the scene component's relative rotation to the input quaternion
void QuatRotations::SetRelativeRotationQuat(USceneComponent* SceneComponent, const FQuat& DesiredRotation)
{
	if (SceneComponent)
	{
		SceneComponent->SetRelativeRotation(DesiredRotation);
	}
}

// Add the input delta rotation to the scene component's current local rotation
void QuatRotations::AddLocalRotationQuat(USceneComponent* SceneComponent, const FQuat& DeltaRotation)
{
	if (SceneComponent)
	{
		SceneComponent->AddLocalRotation(DeltaRotation);
	}
}

// Set the Actor's world rotation to the input quaternion
void QuatRotations::SetActorWorldRotationQuat(AActor* Actor, const FQuat& DesiredRotation)
{
	if (Actor)
	{
		Actor->SetActorRotation(DesiredRotation);
	}
}

// Set the Actor's relative rotation to the input quaternion
void QuatRotations::SetActorRelativeRotationQuat(AActor* Actor, const FQuat& DesiredRotation)
{
	if (Actor)
	{
		Actor->SetActorRelativeRotation(DesiredRotation);
	}
}

// Add the input delta rotation to the Actor's current local rotation
void QuatRotations::AddActorLocalRotationQuat(AActor* Actor, const FQuat& DeltaRotation)
{
	if (Actor)
	{
		Actor->AddActorLocalRotation(DeltaRotation);
	}
}