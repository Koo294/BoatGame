// Animation variables for the bouy

#include "CustomMeshTest.h"
#include "BuoyAnimInstance.h"


// Ensure rot is in the -180 to 180 range
void UBuoyAnimInstance::ClampRot(float & Rot)
{
	while (Rot > 180)
	{
		Rot -= 360;
	}
	while (Rot < -180)
	{
		Rot += 360;
	}
}

// Input wind rotation
void UBuoyAnimInstance::SetWind(float Rot)
{
	WindRot = Rot;

	ClampRot(WindRot);
}

// Input North rotation
void UBuoyAnimInstance::SetNorth(float Rot)
{
	NorthRot = Rot;

	ClampRot(NorthRot);
}

