// Boat animation variables

#pragma once

#include "Animation/AnimInstance.h"
#include "BoatAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMMESHTEST_API UBoatAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
private:

	// Ensure rot is in the -180 to 180 range
	void ClampRot(float &Rot);

protected:

	//boat part rotations/powers

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PartRotations)
	float RudderPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PartRotations)
	float SailPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PartRotations)
	float WindPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PartRotations)
	float WindRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PartRotations)
	float JibWindPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PartRotations)
	float JibWindRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PartRotations)
	float LensYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PartRotations)
	float LensPitch;

	//oars

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rowing)
	bool OarsOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rowing)
	float RowSpeed;

public:

	// Input wind power and rotation for main sail
	void SetWind(float Rot, float Power);

	// Input wind power and rotation for jib sail
	void SetJibWind(float Rot, float Power);

	// Input main sail rotation
	void SetSail(float Rot);

	// Input rudder rotation
	void SetRudder(float Rot);

	// Input rowing speed
	void SetRowSpeed(float Power);

	// Input oar positon
	void SetOarsOut(bool Out);

	// Input lens rotation
	void SetLens(float Yaw, float Pitch);
};
