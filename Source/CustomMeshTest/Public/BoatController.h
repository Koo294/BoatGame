// Handles player input to control the player's boat and camera

#pragma once

#include "GameFramework/PlayerController.h"
#include "BoatController.generated.h"

/**
 * 
 */

//enum for touch locations
UENUM(BlueprintType)
enum class ELastTouchType : uint8 { ELT_View, ELT_Rudder, ELT_Sail, ELT_JibSail, ELT_Oar, ELT_Lens };

UCLASS()
class CUSTOMMESHTEST_API ABoatController : public APlayerController
{
	GENERATED_BODY()
	
private:

	//possessed boat
	class ABoat* DrivingBoat;

	//skysphere that follows player
	class ASkySphere* SkySphere;

	//ocean that follows player
	class AOcean* Ocean;

	// Handle moving and rotating ocean so that it is always filling the player's view
	void HandleMoveOcean();

	//old touch locations and types

	TArray<FVector2D> PrevTouch;

	TArray<FVector2D> PrevTouchBegan;

	TArray<ELastTouchType> LastTouch;

	TArray<bool> ActiveTouch;

	//old boat part rotations
	float PrevSteerSail;

	float PrevSteerLensYaw;

	float PrevSteerLensPitch;

	// Check if a touch is not already using this type
	bool CheckTouchTypeAvailible(ELastTouchType Type);

	//double touch timers

	float SailDoubleTouchTimer;

	float JibSailDoubleTouchTimer;

	float OarDoubleTouchTimer;

	float LensDoubleTouchTimer;

	// Calculate the distance swiped in a direction relative to the 3d world 
	float SwipeAmountFromWorld(FVector2D SwipeStart, FVector2D SwipeEnd, FVector WorldLocation, FRotator WorldRotation);

protected:

	// Handle all touch related input
	virtual bool InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D &TouchLocation, FDateTime DeviceTimestamp, uint32 TouchpadIndex) override;

public:

	// Sets default values for this actor's properties
	ABoatController(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds);

	//view parameters
	float ViewTurnSpeed;
	 
	//rudder parameters

	float RudderTouchRadius;

	float SteerStrength;

	//sail parameters

	float SailTouchRadius;

	float JibSailTouchRadius;

	float SteerSailStrength;

	float SailDoubleTouchTime;

	float JibSailDoubleTouchTime;

	//oar parameters

	float OarInTouchRadius;

	float OarOutTouchRadius;

	float OarDoubleTouchTime;

	float PowerOarStrength;

	//lens parameters

	float SteerLensStrength;

	float LensTouchRadius;

	float LensDoubleTouchTime;
};
