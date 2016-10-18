// Mathematical representation of a single wave form, provides a function to calculate the normal and displacement of the wave at any given location and time.

#pragma once

#include "Object.h"
#include "WaveForm.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta = (ShortTooltip = "Wave form that will combined with other wave forms in the WaveManager to create all ocean waves."))
class CUSTOMMESHTEST_API UWaveForm : public UObject
{
	GENERATED_BODY()
	
private:

	//game wave manager
	class UWaveManager* WaveManager;

public:

	// Sets default values for this actor's properties
	UWaveForm();

	// Find the wave's displacement and normal given a position and time
	virtual void GetWaveDisplacementNormal(FVector2D Position, float Time, FVector &Displacement, FVector &Normal);

	// Called after the wave is created
	virtual void Init(class UWaveManager* Manager);

	// Remove this wave from the wave manager
	void Remove();

	// Add this wave to the wave manager
	void Add();
};
