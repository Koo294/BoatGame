//Uses Gerstner wave functions to create a realistic ocean wave pattern

#pragma once

#include "WaveForm.h"
#include "GerstnerWaveForm.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMMESHTEST_API UGerstnerWaveForm : public UWaveForm
{
	GENERATED_BODY()

private:

	//calculated wave functions

	float lambda;

	FVector2D rotatedDirection;

public:

	// Sets default values for this actor's properties
	UGerstnerWaveForm();

	// Called after the wave is created
	virtual void Init(class UWaveManager* Manager) override;

	//waveform parameters
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WaveSetup)
	float WaveLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WaveSetup)
	float Steepness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WaveSetup)
	float Amplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WaveSetup)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WaveSetup)
	float Phase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WaveSetup)
	FVector2D Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WaveSetup)
	float RotationAngle;

	// Find the wave's displacement and normal given a position and time
	virtual void GetWaveDisplacementNormal(FVector2D Position, float Time, FVector &Displacement, FVector &Normal) override;
	
};
