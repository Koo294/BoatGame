//Uses Gerstner wave functions to create a realistic ocean wave pattern

#include "CustomMeshTest.h"
#include "GerstnerWaveForm.h"

UGerstnerWaveForm::UGerstnerWaveForm()
{
	//defualt waveform params

	WaveLength = 750;

	Steepness = 1;

	Amplitude = 60;

	Speed = 1;

	Phase = 0;

	Direction = FVector2D(2, 0);

	RotationAngle = 0;
}

// Called after the wave is created
void UGerstnerWaveForm::Init(class UWaveManager* Manager)
{
	Super::Init(Manager);

	//initialise calculated wave functions

	lambda = (2 * PI) / WaveLength;

	FVector dir = FVector(Direction.X, Direction.Y, 0);
	dir = dir.RotateAngleAxis(RotationAngle, FVector(0, 0, 1));
	dir.Normalize();

	rotatedDirection = FVector2D(dir.X, dir.Y);

}

// Find the wave's displacement and normal given a position and time
void UGerstnerWaveForm::GetWaveDisplacementNormal(FVector2D Position, float Time, FVector &Displacement, FVector &Normal)
{
	//calculate phase of the wave
	float wavePhase = lambda * FVector2D::DotProduct(rotatedDirection, Position) + (Time*Speed + Phase);

	float c = 0;
	float s = 0;
	
	//calculate sin and cos of the phase
	FMath::SinCos(&s, &c, wavePhase);

	//calculate gerstner wave displacement
	Displacement = FVector(Steepness * Amplitude * rotatedDirection.X * c, Steepness * Amplitude * rotatedDirection.Y * c, Amplitude * s);

	//calculate gerstner wave normal
	Normal = FVector(lambda * Amplitude * rotatedDirection.X * c, lambda * Amplitude * rotatedDirection.Y * c, lambda * Steepness * Amplitude * s);
}
