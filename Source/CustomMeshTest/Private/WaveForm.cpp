// Mathematical representation of a single wave form, provides a function to calculate the normal and displacement of the wave at any given location and time.

#include "CustomMeshTest.h"
#include "WaveManager.h"
#include "CustomMeshTestGameMode.h"
#include "WaveForm.h"

// Sets default values for this actor's properties
UWaveForm::UWaveForm()
{
	//
}

// Find the wave's displacement and normal given a position and time
void UWaveForm::GetWaveDisplacementNormal(FVector2D Position, float Time, FVector &Displacement, FVector &Normal)
{
	return;
}

// Called after the wave is created
void UWaveForm::Init(UWaveManager * Manager)
{
	WaveManager = Manager;
}

// Remove this wave from the wave manager
void UWaveForm::Remove()
{
	if (WaveManager)
	{
		WaveManager->RemoveWaveForm(this);
	}
}

// Add this wave to the wave manager
void UWaveForm::Add()
{
	//get game wave manager
	if (!WaveManager)
	{
		AGameMode* GameMode = UGameplayStatics::GetGameMode(this);
		if (GameMode) Init(Cast<ACustomMeshTestGameMode>(GameMode)->GetWaveManager());
		if (!WaveManager) return;
	}
	WaveManager->AddWaveForm(this);
}
