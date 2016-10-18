// Uses a collection of WaveForms to create a single wave function for the ocean that can calculate the normal and displacement of the waves at any given location and time.

#pragma once

#include "Object.h"
#include "WaveManager.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMMESHTEST_API UWaveManager : public UObject
{
	GENERATED_BODY()

private:

	//wave forms currently used
	UPROPERTY()
	TArray<class UWaveForm*> WaveForms;

public:
	
	// Add a wave form to be used
	UFUNCTION(BlueprintCallable, Category = Waves)
	void AddWaveForm(class UWaveForm* WaveFormAdd);

	// Remove a wave currently used and destroy it
	UFUNCTION(BlueprintCallable, Category = Waves)
	void RemoveWaveForm(class UWaveForm* WaveFormRemove);

	// Find the overall displacement and normal of all waves given a position and time
	void GetWaveDisplacementNormal(FVector2D Position, float Time, FVector &Displacement, FVector &Normal);
};
