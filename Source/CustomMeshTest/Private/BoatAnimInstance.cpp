// Boat animation variables

#include "CustomMeshTest.h"
#include "BoatAnimInstance.h"


// Ensure rot is in the -180 to 180 range
void UBoatAnimInstance::ClampRot(float & Rot)
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

// Input wind power and rotation for main sail
void UBoatAnimInstance::SetWind(float Rot, float Power)
{
	WindRot = Rot;
	WindPower = Power;

	ClampRot(WindRot);
}


// Input wind power and rotation for jib sail
void UBoatAnimInstance::SetJibWind(float Rot, float Power)
{
	JibWindRot = Rot;
	JibWindPower = Power;

	ClampRot(JibWindRot);
}

// Input main sail rotation
void UBoatAnimInstance::SetSail(float Rot)
{
	SailPos = Rot;

	ClampRot(SailPos);
}


// Input rudder rotation
void UBoatAnimInstance::SetRudder(float Rot)
{
	RudderPos = Rot;

	ClampRot(RudderPos);
}

// Input rowing speed
void UBoatAnimInstance::SetRowSpeed(float Power)
{
	RowSpeed = Power;
}

// Input oar positon
void UBoatAnimInstance::SetOarsOut(bool Out)
{
	OarsOut = Out;
}

// Input lens rotation
void UBoatAnimInstance::SetLens(float Yaw, float Pitch)
{
	LensYaw = Yaw;

	ClampRot(LensYaw);

	LensPitch = Pitch;

	ClampRot(LensPitch);
}
