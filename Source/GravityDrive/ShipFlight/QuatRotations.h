// ©, 2021, Erik Jastad. All Rights Reserved

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
namespace QuatRotations
{
	// Convert Euler Rotations To Quaternions
	FQuat EulerToQuaternion(FRotator CurrentRotation);

	// Function to set world rotation of scene component to input quaternion rotation	
	void SetWorldRotationQuat(USceneComponent* SceneComponent, const FQuat& DesiredRotation);

	// Function to set relative rotation of scene component to input quaternion rotation
	void SetRelativeRotationQuat(USceneComponent* SceneComponent, const FQuat& DesiredRotation);

	// Function to add delta rotation to current local rotation of scene component
	void AddLocalRotationQuat(USceneComponent* SceneComponent, const FQuat& DeltaRotation);

	// Function to set world rotation of Actor to input quaternion rotation
	void SetActorWorldRotationQuat(AActor* Actor, const FQuat& DesiredRotation);

	// Function to set relative rotation of Actor to input quaternion rotation
	void SetActorRelativeRotationQuat(AActor* Actor, const FQuat& DesiredRotation);

	// Function to add delta rotation to current local rotation of Actor
	void AddActorLocalRotationQuat(AActor* Actor, const FQuat& DeltaRotation);
}