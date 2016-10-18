// Animation variables for the bouy

#pragma once

#include "Animation/AnimInstance.h"
#include "BuoyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMMESHTEST_API UBuoyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:

	// Ensure rot is in the -180 to 180 range
	void ClampRot(float &Rot);

protected:

	//part rotations

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PartRotations)
	float WindRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PartRotations)
	float NorthRot;
	
public:
	
	// Input wind rotation
	void SetWind(float Rot);

	// Input North rotation
	void SetNorth(float Rot);
	
};
