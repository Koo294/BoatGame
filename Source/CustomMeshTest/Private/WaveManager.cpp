// Uses a collection of WaveForms to create a single wave function for the ocean that can calculate the normal and displacement of the waves at any given location and time.

#include "CustomMeshTest.h"
#include "WaveForm.h"
#include "WaveManager.h"


// Add a wave form to be used
void UWaveManager::AddWaveForm(UWaveForm * WaveFormAdd)
{
	if (!WaveFormAdd) return;
	WaveFormAdd->Init(this);
	WaveForms.Add(WaveFormAdd);
}


// Remove a wave currently used and destroy it
void UWaveManager::RemoveWaveForm(UWaveForm * WaveFormRemove)
{
	if (!WaveFormRemove) return;
	if (!WaveForms.Remove(WaveFormRemove)) return;
	if (!WaveFormRemove->IsValidLowLevel()) return;
	WaveFormRemove->ConditionalBeginDestroy();
	WaveFormRemove = NULL;
	GetWorld()->ForceGarbageCollection(true);
}

// Find the overall displacement and normal of all waves given a position and time
void UWaveManager::GetWaveDisplacementNormal(FVector2D Position, float Time, FVector &Displacement, FVector &Normal)
{
	FVector TotalDisplacement = FVector::ZeroVector;
	FVector TotalNormal = FVector::ZeroVector;

	//for all wave forms
	for (int32 i = 0; i < WaveForms.Num(); i++)
	{
		if (WaveForms[i])
		{
			FVector AdditionalDisplacement;
			FVector AdditionalNormal;

			//add displacement and normal of this wave
			WaveForms[i]->GetWaveDisplacementNormal(Position, Time, AdditionalDisplacement, AdditionalNormal);
			TotalDisplacement += AdditionalDisplacement;
			TotalNormal += AdditionalNormal;
		}
	}

	Displacement = TotalDisplacement;

	//correct normal
	Normal = FVector(0 - TotalNormal.X, 0 - TotalNormal.Y, 1 - TotalNormal.Z);
}
