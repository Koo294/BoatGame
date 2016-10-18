// Player boat - sails across the ocean under the player's command

#pragma once

#include "GameFramework/Pawn.h"
#include "Boat.generated.h"

UCLASS()
class CUSTOMMESHTEST_API ABoat : public APawn
{
	GENERATED_BODY()
	
private:

	//rudder/movement

	float RudderPosition;

	float Steering;

	float Power;

	//Handle boat motion in the ocean
	void HandleMovement(float DeltaTime);

	// Handle animated movement of boat parts
	void HandlePartsMovement(float DeltaTime);

	// Handle foam particle systems
	void HandleFoam(float DeltaTime);

	// Handle camera positioning
	void HandleCamera(float DeltaTime);

	//waves
	class UWaveManager* WaveManager;

	//sails

	float SailPosition;

	float SailSteering;

	float SailUp;

	float JibSailUp;

	bool SailGoingUp;

	bool JibSailGoingUp;

	//foam

	float FoamIntervalTimer;

	float SprayTimer;

	//oars

	bool OarsOut;

	float OarPower;

	//lens

	float LensYaw;

	float LensYawSteer;

	float LensPitch;

	float LensPitchSteer;

	bool LensFire;

	//dynamic materials

	UMaterial* SailMaterial;

	UMaterial* RopeMaterial;

	UMaterialInstanceDynamic* MainSailMat;

	UMaterialInstanceDynamic* JibSailMat;

	UMaterialInstanceDynamic* MainRopeMat;

	UMaterialInstanceDynamic* JibRopeMat;

	// Function with falloff, so that light winds give relatively more power than strong winds - to ensure the boat can get going.
	float SailPowerFunction(float Val);

protected:

	//actor components

	UPROPERTY(EditAnywhere, Category = Components)
	USkeletalMeshComponent* BoatMesh;

	UPROPERTY(EditAnywhere, Category = Components)
	class UFloatComponent* FloatComp;

	UPROPERTY(EditAnywhere, Category = Components)
	UParticleSystemComponent* FoamSpray;

	UPROPERTY(EditAnywhere, Category = Components)
	class USplineComponent* FoamSpline;

	UPROPERTY(EditAnywhere, Category = Components)
	class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, Category = Components)
	UStaticMeshComponent* LightBeamMesh;

	//animation
	class UBoatAnimInstance* BoatAnim;

public:	
	// Sets default values for this actor's properties
	ABoat();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//movement parameters

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float RudderResist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float SteeringSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float SailResist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float SailPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float SailBackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float SailMaxRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float JibSailPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float JibSailPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float JibSailBackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float OarPowerMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float OarResist;

	//lens parameters

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lens)
	float LensYawMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lens)
	float LensPitchMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lens)
	float LensPitchMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lens)
	float LensResist;

	//touch location parameters

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TouchLocations)
	FVector RudderOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TouchLocations)
	FRotator RudderSwipeRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TouchLocations)
	FVector SailOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TouchLocations)
	FRotator SailSwipeRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TouchLocations)
	FVector JibSailOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TouchLocations)
	FVector OarInLeftOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TouchLocations)
	FVector OarInRightOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TouchLocations)
	FVector OarOutLeftOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TouchLocations)
	FVector OarOutRightOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TouchLocations)
	FRotator OarSwipeRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TouchLocations)
	FVector LensOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TouchLocations)
	FRotator LensYawSwipeRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TouchLocations)
	FRotator LensPitchSwipeRot;

	//foam particle parameters

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Foam)
	float FoamInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Foam)
	float FoamPassiveAdd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Foam)
	float FoamOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Foam)
	float SprayAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Foam)
	float SprayMinSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Foam)
	float SprayMaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Foam)
	TArray<FVector> SprayCurvePoints;

	//camera parameters

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	FVector CameraOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraTilt;

	// Steering input
	void SteerRudder(float Val);

	// Sail angle input
	void SteerSail(float Val);

	// Lens angle input
	void SteerLens(float Yaw, float Pitch);

	// Oar power input
	void PowerOars(float Val);

	float GetSailPosition();

	void GetLensPosition(float &Yaw, float &Pitch);

	bool GetOarsOut();

	// Oar in/out input
	void SetOarsOut(bool Out);

	// Main sail up/down input
	void SetSailUp(bool Up);

	// Jib sail up/down input
	void SetJibSailUp(bool Up);

	bool GetSailUp();

	bool GetJibSailUp();

	bool GetLensFire();

	// Lens firing input
	void SetLensFire(bool Fire);

	// Below get functions are for getting touch zone locations and swipe rotations

	FVector GetRudderWorldLocation();

	FVector GetSailWorldLocation();

	FVector GetJibSailWorldLocation();

	FVector GetOarInLeftWorldLocation();

	FVector GetOarInRightWorldLocation();

	FVector GetOarOutLeftWorldLocation();

	FVector GetOarOutRightWorldLocation();

	FVector GetCameraWorldLocation();

	FVector GetLensWorldLocation();

	FRotator GetRudderWorldRotation();

	FRotator GetSailWorldRotation();

	FRotator GetOarWorldRotation();

	FRotator GetLensYawWorldRotation();

	FRotator GetLensPitchWorldRotation();
};
