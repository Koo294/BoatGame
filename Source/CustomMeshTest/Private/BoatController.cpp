// Handles player input to control the player's boat and camera

#include "CustomMeshTest.h"
#include "Boat.h"
#include "Ocean.h"
#include "SkySphere.h"
#include "BoatController.h"


// Sets default values for this actor's properties
ABoatController::ABoatController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

	PrimaryActorTick.bCanEverTick = true;

	//view params
	ViewTurnSpeed = 0.2f;

	//rudder params

	RudderTouchRadius = 50.f;

	SteerStrength = 0.0025f;

	//sail params

	SailTouchRadius = 120.f;

	SailDoubleTouchTime = 0.4f;

	JibSailTouchRadius = 60.f;

	JibSailDoubleTouchTime = 0.4f;

	SteerSailStrength = 0.2f;

	//oar params

	OarInTouchRadius = 50.f;

	OarOutTouchRadius = 90.f;

	OarDoubleTouchTime = 0.4f;

	PowerOarStrength = 0.005f;

	//lens params

	LensTouchRadius = 50.f;

	SteerLensStrength = 0.6f;

	LensDoubleTouchTime = 0.4f;

	//prev touch initialization

	PrevTouch.SetNumUninitialized(5);

	PrevTouchBegan.SetNumUninitialized(5);

	LastTouch.SetNumUninitialized(5);

	ActiveTouch.SetNum(5);
}

// Called when the game starts or when spawned
void ABoatController::BeginPlay()
{
	Super::BeginPlay();


	//get possessed boat
	DrivingBoat = Cast<ABoat>(GetPawn());

	if (DrivingBoat)
		{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		//create ocean
		Ocean = GetWorld()->SpawnActor<AOcean>(DrivingBoat->GetActorLocation(), DrivingBoat->GetActorRotation(), SpawnParams);

		//create sky sphere
		SkySphere = GetWorld()->SpawnActor<ASkySphere>(DrivingBoat->GetActorLocation(), DrivingBoat->GetActorRotation(), SpawnParams);
	}
}

// Handle moving and rotating ocean so that it is always filling the player's view
void ABoatController::HandleMoveOcean()
{
	if (Ocean)
	{
		//move ocean to boat location (only x and y)
		Ocean->SetActorLocation(FVector(DrivingBoat->GetActorLocation().X, DrivingBoat->GetActorLocation().Y, Ocean->GetActorLocation().Z));
		//rotate ocean to controller (camera) rotation
		Ocean->SetActorRotation(FRotator(0.f, GetControlRotation().Yaw, 0.f));
	}
	if (SkySphere)
	{
		//move skysphere to boat location
		SkySphere->SetActorLocation(DrivingBoat->GetActorLocation());
	}
}

// Check if a touch is not already using this type
bool ABoatController::CheckTouchTypeAvailible(ELastTouchType Type)
{
	//go through all possible touches
	for (int32 i = 0; i < ActiveTouch.Num(); i++)
	{
		//if this touch is active and of the same type
		if (ActiveTouch[i] && LastTouch[i] == Type)
		{
			//not availible
			return false;
		}
	}

	//no active touches using this type, so it's availible
	return true;
}

// Calculate the distance swiped in a direction relative to the 3d world 
float ABoatController::SwipeAmountFromWorld(FVector2D SwipeStart, FVector2D SwipeEnd, FVector WorldLocation, FRotator WorldRotation)
{
	//project starting world location and world location along rotation's normal to screen space
	FVector2D WorldStart;
	UGameplayStatics::ProjectWorldToScreen(this, WorldLocation, WorldStart);
	FVector2D WorldEnd;
	UGameplayStatics::ProjectWorldToScreen(this, WorldLocation+(WorldRotation.Vector()), WorldEnd);

	//dot to find swipe amount in that direction in screen space
	return (WorldEnd - WorldStart) | (SwipeEnd - SwipeStart);
}

// Handle all touch related input
bool ABoatController::InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D &TouchLocation, FDateTime DeviceTimestamp, uint32 TouchpadIndex)
{
	//can't input if no boat
	if (!DrivingBoat) return false;

	switch (Type)
	{
		//initial touch
		case ETouchType::Began:
		{
			//default to view
			LastTouch[Handle] = ELastTouchType::ELT_View;
			//closer things have more priority, so set default to a large number (lowest priority)
			float PriorityDepth = 10000;

			//rudder

			FVector2D RudderScreenPos;
			//get screen space location of rudder
			UGameplayStatics::ProjectWorldToScreen(this, DrivingBoat->GetRudderWorldLocation(), RudderScreenPos);
			//get depth of rudder from camera
			float RudderCamDepth = (DrivingBoat->GetCameraWorldLocation() - DrivingBoat->GetRudderWorldLocation()).Size();

			//touch location is within rudder touch radius and depth takes priority
			if ((RudderScreenPos - TouchLocation).Size() < RudderTouchRadius && RudderCamDepth < PriorityDepth)
			{
				//set as new type to use and set new priority depth
				LastTouch[Handle] = ELastTouchType::ELT_Rudder;
				PriorityDepth = RudderCamDepth;
			}

			//sail
			FVector2D SailScreenPos;
			//get screen space location of main sail
			UGameplayStatics::ProjectWorldToScreen(this, DrivingBoat->GetSailWorldLocation(), SailScreenPos);
			//get depth of main sail from camera
			float SailCamDepth = (DrivingBoat->GetCameraWorldLocation() - DrivingBoat->GetSailWorldLocation()).Size();

			//touch location is within main sail touch radius and depth takes priority
			if ((SailScreenPos - TouchLocation).Size() < SailTouchRadius && SailCamDepth < PriorityDepth)
			{
				//set as new type to use and set new priority depth
				LastTouch[Handle] = ELastTouchType::ELT_Sail;
				PriorityDepth = SailCamDepth;
			}

			//jibsail
			FVector2D JibSailScreenPos;
			//get screen space location of jib sail
			UGameplayStatics::ProjectWorldToScreen(this, DrivingBoat->GetJibSailWorldLocation(), JibSailScreenPos);
			//get depth of jib sail from camera
			float JibSailCamDepth = (DrivingBoat->GetCameraWorldLocation() - DrivingBoat->GetJibSailWorldLocation()).Size();

			//touch location is within jib sail touch radius and depth takes priority
			if ((JibSailScreenPos - TouchLocation).Size() < JibSailTouchRadius && JibSailCamDepth < PriorityDepth)
			{
				//set as new type to use and set new priority depth
				LastTouch[Handle] = ELastTouchType::ELT_JibSail;
				PriorityDepth = JibSailCamDepth;
			}

			//lens
			FVector2D LensScreenPos;
			//get screen space location of lens
			UGameplayStatics::ProjectWorldToScreen(this, DrivingBoat->GetLensWorldLocation(), LensScreenPos);
			//get depth of lens from camera
			float LensCamDepth = (DrivingBoat->GetCameraWorldLocation() - DrivingBoat->GetLensWorldLocation()).Size();

			//touch location is within lens touch radius and depth takes priority
			if ((LensScreenPos - TouchLocation).Size() < LensTouchRadius && LensCamDepth < PriorityDepth)
			{
				//set as new type to use and set new priority depth
				LastTouch[Handle] = ELastTouchType::ELT_Lens;
				PriorityDepth = LensCamDepth;
			}

			//oars

			FVector2D OarLeftScreenPos;
			float OarLeftCamDepth = 10000;
			FVector2D OarRightScreenPos;
			float OarRightCamDepth = 10000;
			
			//different world space locations for in or out oars
			if (DrivingBoat->GetOarsOut())
			{
				//get screen space location of oars
				UGameplayStatics::ProjectWorldToScreen(this, DrivingBoat->GetOarOutLeftWorldLocation(), OarLeftScreenPos);
				UGameplayStatics::ProjectWorldToScreen(this, DrivingBoat->GetOarOutRightWorldLocation(), OarRightScreenPos);
				//get depth of oars from camera
				OarLeftCamDepth = (DrivingBoat->GetCameraWorldLocation() - DrivingBoat->GetOarOutLeftWorldLocation()).Size();
				OarRightCamDepth = (DrivingBoat->GetCameraWorldLocation() - DrivingBoat->GetOarOutRightWorldLocation()).Size();
			}
			else
			{
				//get screen space location of oars
				UGameplayStatics::ProjectWorldToScreen(this, DrivingBoat->GetOarInLeftWorldLocation(), OarLeftScreenPos);
				UGameplayStatics::ProjectWorldToScreen(this, DrivingBoat->GetOarInRightWorldLocation(), OarRightScreenPos);
				//get depth of oars from camera
				OarLeftCamDepth = (DrivingBoat->GetCameraWorldLocation() - DrivingBoat->GetOarInLeftWorldLocation()).Size();
				OarRightCamDepth = (DrivingBoat->GetCameraWorldLocation() - DrivingBoat->GetOarInRightWorldLocation()).Size();
			}

			//touch location is within left oar touch radius and depth takes priority
			if ((OarLeftScreenPos - TouchLocation).Size() < (DrivingBoat->GetOarsOut() ? OarOutTouchRadius : OarInTouchRadius) && OarLeftCamDepth < PriorityDepth)
			{
				//set as new type to use and set new priority depth
				LastTouch[Handle] = ELastTouchType::ELT_Oar;
				PriorityDepth = OarLeftCamDepth;
			}

			//touch location is within right oar touch radius and depth takes priority
			if ((OarRightScreenPos - TouchLocation).Size() < (DrivingBoat->GetOarsOut() ? OarOutTouchRadius : OarInTouchRadius) && OarRightCamDepth < PriorityDepth)
			{
				//set as new type to use and set new priority depth
				LastTouch[Handle] = ELastTouchType::ELT_Oar;
				PriorityDepth = OarRightCamDepth;
			}


			//
			//check if selected touch type is in use and if so set active touch for this touch
			ActiveTouch[Handle] = CheckTouchTypeAvailible(LastTouch[Handle]);

			if (ActiveTouch[Handle])
			{
				PrevTouchBegan[Handle] = TouchLocation;

				switch (LastTouch[Handle])
				{
					case ELastTouchType::ELT_Sail:
					{
						if (SailDoubleTouchTimer <= 0.f)
						{
							//get current sail position so it's not reset
							PrevSteerSail = DrivingBoat->GetSailPosition();
							//start timer for double touch
							SailDoubleTouchTimer = SailDoubleTouchTime;
							//if the sail is up, treat this touch as a view touch rather than a sail touch
							if (!DrivingBoat->GetSailUp() && CheckTouchTypeAvailible(ELastTouchType::ELT_View)) LastTouch[Handle] = ELastTouchType::ELT_View;
						}
						else
						{
							//on double touch put main sail up/down
							DrivingBoat->SetSailUp(!DrivingBoat->GetSailUp());
						}
						break;
					}
					case ELastTouchType::ELT_JibSail:
					{
						if (JibSailDoubleTouchTimer <= 0.f)
						{
							//start timer for double touch
							JibSailDoubleTouchTimer = JibSailDoubleTouchTime;
							//if the sail is up, treat this touch as a view touch rather than a sail touch
							if (!DrivingBoat->GetJibSailUp() && CheckTouchTypeAvailible(ELastTouchType::ELT_View)) LastTouch[Handle] = ELastTouchType::ELT_View;
						}
						else
						{
							//on double touch put jib sail up/down
							DrivingBoat->SetJibSailUp(!DrivingBoat->GetJibSailUp());
						}
						break;
					}
					case ELastTouchType::ELT_Lens:
					{
						if (LensDoubleTouchTimer <= 0.f)
						{
							//start timer for double touch
							LensDoubleTouchTimer = LensDoubleTouchTime;
						}
						else
						{
							//on double touch set lens to fire/stop firing
							DrivingBoat->SetLensFire(!DrivingBoat->GetLensFire());
						}
						DrivingBoat->GetLensPosition(PrevSteerLensYaw, PrevSteerLensPitch);
						break;
					}
					case ELastTouchType::ELT_Oar:
					{
						if (OarDoubleTouchTimer <= 0.f)
						{
							//start timer for double touch
							OarDoubleTouchTimer = OarDoubleTouchTime;
						}
						else
						{
							//on double touch bring oars out/in
							DrivingBoat->SetOarsOut(!DrivingBoat->GetOarsOut());
						}
						break;
					}
				}
			}
			break;
		}
		//touch moved
		case ETouchType::Moved:
		{
			if (ActiveTouch[Handle])
			{
				switch (LastTouch[Handle])
				{
					case ELastTouchType::ELT_Rudder:
					{
						//input boat steering from touch swipe
						DrivingBoat->SteerRudder(SwipeAmountFromWorld(PrevTouchBegan[Handle], TouchLocation, DrivingBoat->GetRudderWorldLocation(), DrivingBoat->GetRudderWorldRotation()) * SteerStrength);
						break;
					}
					case ELastTouchType::ELT_Sail:
					{
						//input main sail steering from touch swipe
						DrivingBoat->SteerSail((SwipeAmountFromWorld(PrevTouchBegan[Handle], TouchLocation, DrivingBoat->GetSailWorldLocation(), DrivingBoat->GetSailWorldRotation()) * SteerSailStrength) + PrevSteerSail);
						break;
					}
					case ELastTouchType::ELT_Lens:
					{
						//input lens steering from touch swipe
						DrivingBoat->SteerLens((SwipeAmountFromWorld(PrevTouchBegan[Handle], TouchLocation, DrivingBoat->GetLensWorldLocation(), DrivingBoat->GetLensYawWorldRotation()) * SteerLensStrength) + PrevSteerLensYaw, (SwipeAmountFromWorld(PrevTouchBegan[Handle], TouchLocation, DrivingBoat->GetLensWorldLocation(), DrivingBoat->GetLensPitchWorldRotation()) * SteerLensStrength) + PrevSteerLensPitch);
						break;
					}
					case ELastTouchType::ELT_Oar:
					{
						//input oar power from touch swipe
						DrivingBoat->PowerOars(SwipeAmountFromWorld(PrevTouchBegan[Handle], TouchLocation, DrivingBoat->GetActorLocation(), DrivingBoat->GetOarWorldRotation()) * PowerOarStrength);
						break;
					}
					case ELastTouchType::ELT_View:
					{
						//input view rotation from touch swipe
						AddYawInput((TouchLocation.X - PrevTouch[Handle].X) * ViewTurnSpeed);
						AddPitchInput((TouchLocation.Y - PrevTouch[Handle].Y) * ViewTurnSpeed);
						break;
					}
				}
			}
			break; 
		}
		//touch release
		case ETouchType::Ended:
		{
			if (ActiveTouch[Handle])
			{
				//stop active touch
				ActiveTouch[Handle] = false;
				//reset rudder so that when not swiping boat steers straight
				if (LastTouch[Handle] == ELastTouchType::ELT_Rudder)
				{
					DrivingBoat->SteerRudder(0);

				}
			}
			break;
		}
	}

	//FString DebugString = FString();
	//DebugString.AppendInt(Handle);
	//DrawDebugString(GetWorld(), GetPawn()->GetActorLocation() + FVector(0, 0, 400), DebugString, (AActor *)0, FColor::Red, 0.1f);

	PrevTouch[Handle] = TouchLocation;
	return true;
}

// Called every frame
void ABoatController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (DrivingBoat)
	{
		HandleMoveOcean();
	}

	//handle double touch timers

	if (OarDoubleTouchTimer > 0.f)
	{
		OarDoubleTouchTimer -= DeltaSeconds;
	}
	if (SailDoubleTouchTimer > 0.f)
	{
		SailDoubleTouchTimer -= DeltaSeconds;
	}
	if (JibSailDoubleTouchTimer > 0.f)
	{
		JibSailDoubleTouchTimer -= DeltaSeconds;
	}
	if (LensDoubleTouchTimer > 0.f)
	{
		LensDoubleTouchTimer -= DeltaSeconds;
	}
}