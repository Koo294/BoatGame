// Player boat - sails across the ocean under the player's command

#include "CustomMeshTest.h"
#include "WaveManager.h"
#include "CustomMeshTestGameMode.h"
#include "BoatAnimInstance.h"
#include "FoamDecal.h"
#include "Classes/Components/SplineComponent.h"
#include "FloatComponent.h"
#include "Boat.h"


// Sets default values
ABoat::ABoat()
{
	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, false);

	//boat mesh and anims
	BoatMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BoatMesh"));
	RootComponent = BoatMesh;

	//find mesh and animation
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BoatMeshClassFinder(TEXT("/Game/Boat/Models/testboat"));
	static ConstructorHelpers::FClassFinder<UAnimInstance> BoatAnimClassFinder(TEXT("/Game/Boat/Animations/TestBoatAnimBP"));
	BoatMesh->SetSkeletalMesh(BoatMeshClassFinder.Object);
	BoatMesh->SetAnimInstanceClass(BoatAnimClassFinder.Class);

	//lens light beam mesh
	LightBeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightBeamMesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> LightBeamMeshClassFinder(TEXT("/Game/Boat/Models/lightbeam"));
	LightBeamMesh->SetStaticMesh(LightBeamMeshClassFinder.Object);
	LightBeamMesh->SetVisibility(false);

	LightBeamMesh->AttachToComponent(RootComponent, AttachRules, "lensFire");

	//float physics
	FloatComp = CreateDefaultSubobject<UFloatComponent>(TEXT("FloatComp"));
	FloatComp->SideDrag = 0.8f;

	//foam particle system
	FoamSpray = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FoamSpray"));
	FoamSpray->AttachToComponent(RootComponent, AttachRules);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> FoamSprayClassFinder(TEXT("/Game/Boat/Effects/FoamSprayParticle"));
	FoamSpray->SetTemplate(FoamSprayClassFinder.Object);

	//spline along which foam spawns
	FoamSpline = CreateDefaultSubobject<USplineComponent>(TEXT("FoamSpline"));
	FoamSpline->AttachToComponent(RootComponent, AttachRules);

	//camera view
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->AttachToComponent(RootComponent, AttachRules);

	//sail materials
	static ConstructorHelpers::FObjectFinder<UMaterial> SailMatClassFinder(TEXT("/Game/Boat/Materials/testboatsailmat"));
	SailMaterial = SailMatClassFinder.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial> RopeMatClassFinder(TEXT("/Game/Boat/Materials/boatropes"));
	RopeMaterial = RopeMatClassFinder.Object;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//steering params
	RudderResist = 0.5f;
	SteeringSpeed = 0.1f;

	//sail/power params
	Power = 0.f;
	SailPower = 9.f;
	SailBackPower = 3.f;
	SailResist = 0.25f;
	SailMaxRotation = 90.f;
	JibSailPower = 3.5f;
	JibSailBackPower = 0.5f;
	JibSailPosition = -40.f;
	SailUp = 1.f;
	SailGoingUp = true;
	JibSailUp = 1.f;
	JibSailGoingUp = true;

	//lens params
	LensResist = 0.3f;
	LensYawMax = 90.f;
	LensPitchMax = 80.f;
	LensPitchMin = -20.f;

	//oar params
	OarPowerMax = 50;
	OarResist = 0.65f;

	//foam particle params
	FoamInterval = 400.f;
	FoamPassiveAdd = 80.f;
	FoamOffset = 1.f;
	SprayAmount = 0.2f;
	SprayMaxSpeed = 5.f;
	SprayMinSpeed = 2.f;

	//foam spline points
	SprayCurvePoints.Add(FVector(-90.f, -50.f, 10.f));
	SprayCurvePoints.Add(FVector(70.f, -50.f, 10.f));
	SprayCurvePoints.Add(FVector(170.f, 0.f, 10.f));
	SprayCurvePoints.Add(FVector(70.f, 50.f, 10.f));
	SprayCurvePoints.Add(FVector(-90.f, 50.f, 10.f));

	//camera params
	CameraOffset = FVector(0.f, 0.f, 30.f);
	CameraDistance = 600.f;
	CameraTilt = 7.5f;
}

// Called when the game starts or when spawned
void ABoat::BeginPlay()
{
	Super::BeginPlay();

	//set anim for code access
	BoatAnim = Cast<UBoatAnimInstance>(BoatMesh->GetAnimInstance());

	//dynamic materials
	MainSailMat = UMaterialInstanceDynamic::Create(SailMaterial, this);
	JibSailMat = UMaterialInstanceDynamic::Create(SailMaterial, this);

	MainRopeMat = UMaterialInstanceDynamic::Create(RopeMaterial, this);
	JibRopeMat = UMaterialInstanceDynamic::Create(RopeMaterial, this);

	BoatMesh->SetMaterial(0, MainSailMat);
	BoatMesh->SetMaterial(3, JibSailMat);

	BoatMesh->SetMaterial(4, MainRopeMat);
	BoatMesh->SetMaterial(2, JibRopeMat);

	//create foam spline
	FoamSpline->SetSplineLocalPoints(SprayCurvePoints);

}

// Called every frame
void ABoat::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	//handle all the things
	HandlePartsMovement(DeltaTime);
	HandleMovement(DeltaTime);
	HandleFoam(DeltaTime);
	HandleCamera(DeltaTime);
}

// Function with falloff, so that light winds give relatively more power than strong winds - to ensure the boat can get going.
float ABoat::SailPowerFunction(float Val)
{
	if (Val != 0.f)
	{
		return (Val * 10) / FMath::Sqrt(FMath::Abs(Val));
	}
	return 0.f;
}

// Steering input
void ABoat::SteerRudder(float Val)
{
	Steering = Val;
}

// Sail angle input
void ABoat::SteerSail(float Val)
{
	SailSteering = Val;

	//prevent steering out of bounds
	if (SailSteering > SailMaxRotation)
	{
		SailSteering = SailMaxRotation;
	}
	else if (SailSteering < -SailMaxRotation)
	{
		SailSteering = -SailMaxRotation;
	}
}

// Lens angle input
void ABoat::SteerLens(float Yaw, float Pitch)
{
	LensYawSteer = Yaw;

	//prevent steering yaw out of bounds
	if (LensYawSteer > LensYawMax)
	{
		LensYawSteer = LensYawMax;
	}
	else if (LensYawSteer < -LensYawMax)
	{
		LensYawSteer = -LensYawMax;
	}

	LensPitchSteer = Pitch;

	//prevent steering pitch out of bounds
	if (LensPitchSteer > LensPitchMax)
	{
		LensPitchSteer = LensPitchMax;
	}
	else if (LensPitchSteer < LensPitchMin)
	{
		LensPitchSteer = LensPitchMin;
	}
}

// Oar power input
void ABoat::PowerOars(float Val)
{
	//only pass value if oars are out - or if power is set to 0 for a reset
	if (OarsOut || Val == 0.f) OarPower = Val*OarPowerMax;

	//pass on to anim
	if (BoatAnim)
	{
		BoatAnim->SetRowSpeed(Val);
	}
}

float ABoat::GetSailPosition()
{
	return SailPosition;
}

void ABoat::GetLensPosition(float & Yaw, float & Pitch)
{
	Yaw = LensYaw;
	Pitch = LensPitch;
}

bool ABoat::GetOarsOut()
{
	return OarsOut;
}

// Oar in/out input
void ABoat::SetOarsOut(bool Out)
{
	OarsOut = Out;
	//reset oar power so oars don't come out at full speed
	PowerOars(0);

	//pass on to anim
	if (BoatAnim)
	{
		BoatAnim->SetOarsOut(Out);
	}
}

// Main sail up/down input
void ABoat::SetSailUp(bool Up)
{
	SailGoingUp = Up;
}

// Jib sail up/down input
void ABoat::SetJibSailUp(bool Up)
{
	JibSailGoingUp = Up;
}

bool ABoat::GetSailUp()
{
	return SailGoingUp;
}

bool ABoat::GetJibSailUp()
{
	return JibSailGoingUp;
}

bool ABoat::GetLensFire()
{
	return LensFire;
}

// Lens firing input
void ABoat::SetLensFire(bool Fire)
{
	LensFire = Fire;
	LightBeamMesh->SetVisibility(Fire);
}

// Below get functions are for getting touch zone locations and swipe rotations

FVector ABoat::GetRudderWorldLocation()
{
	return GetActorLocation() + GetActorRotation().RotateVector(RudderOffset);
}

FVector ABoat::GetSailWorldLocation()
{
	return GetActorLocation() + GetActorRotation().RotateVector(SailOffset);
}

FVector ABoat::GetJibSailWorldLocation()
{
	return GetActorLocation() + GetActorRotation().RotateVector(JibSailOffset);
}

FVector ABoat::GetOarInLeftWorldLocation()
{
	return GetActorLocation() + GetActorRotation().RotateVector(OarInLeftOffset);
}

FVector ABoat::GetOarInRightWorldLocation()
{
	return GetActorLocation() + GetActorRotation().RotateVector(OarInRightOffset);
}

FVector ABoat::GetOarOutLeftWorldLocation()
{
	return GetActorLocation() + GetActorRotation().RotateVector(OarOutLeftOffset);
}

FVector ABoat::GetOarOutRightWorldLocation()
{
	return GetActorLocation() + GetActorRotation().RotateVector(OarOutRightOffset);
}

FVector ABoat::GetLensWorldLocation()
{
	return GetActorLocation() + GetActorRotation().RotateVector(LensOffset);
}

FVector ABoat::GetCameraWorldLocation()
{
	return CameraComp->GetComponentLocation();
}

FRotator ABoat::GetRudderWorldRotation()
{
	return GetActorRotation() + RudderSwipeRot;
}

FRotator ABoat::GetSailWorldRotation()
{
	return GetActorRotation() + SailSwipeRot + FRotator(0.f, SailPosition, 0.f);
}

FRotator ABoat::GetOarWorldRotation()
{
	return GetActorRotation() + OarSwipeRot;
}

FRotator ABoat::GetLensYawWorldRotation()
{
	return GetActorRotation() + LensYawSwipeRot + FRotator(0.f, LensYaw, 0.f);
}

FRotator ABoat::GetLensPitchWorldRotation()
{
	return GetActorRotation() + LensPitchSwipeRot;
}

// Handle animated movement of boat parts
void ABoat::HandlePartsMovement(float DeltaTime)
{
	//RudderDirection

	RudderPosition = FMath::Lerp(Steering, RudderPosition, FMath::Pow(RudderResist, DeltaTime));
	//pass on to anim
	if (BoatAnim) BoatAnim->SetRudder(RudderPosition);


	//SailDirection

	SailPosition = FMath::Lerp(SailSteering, SailPosition, FMath::Pow(SailResist, DeltaTime));
	//pass on to anim
	if (BoatAnim) BoatAnim->SetSail(SailPosition);


	//Lens

	LensYaw = FMath::Lerp(LensYawSteer, LensYaw, FMath::Pow(LensResist, DeltaTime));
	LensPitch = FMath::Lerp(LensPitchSteer, LensPitch, FMath::Pow(LensResist, DeltaTime));
	//pass on to anim
	if (BoatAnim) BoatAnim->SetLens(LensYaw, LensPitch);

	//SailUp
	if (SailGoingUp && SailUp < 1.f)
	{
		SailUp += DeltaTime;
		if (SailUp > 1.f) SailUp = 1.f;
		//pass on to dynaimc mats
		MainSailMat->SetScalarParameterValue("SailUp", SailUp);
		MainRopeMat->SetScalarParameterValue("Fade", SailUp);
	}
	else if (!SailGoingUp && SailUp > 0.f)
	{
		SailUp -= DeltaTime;
		if (SailUp < 0.f) SailUp = 0.f;
		//pass on to dynaimc mats
		MainSailMat->SetScalarParameterValue("SailUp", SailUp);
		MainRopeMat->SetScalarParameterValue("Fade", SailUp);
	}

	//JibSailUp
	if (JibSailGoingUp && JibSailUp < 1.f)
	{
		JibSailUp += DeltaTime;
		if (JibSailUp > 1.f) JibSailUp = 1.f;
		//pass on to dynaimc mats
		JibSailMat->SetScalarParameterValue("SailUp", JibSailUp);
		JibRopeMat->SetScalarParameterValue("Fade", JibSailUp);
	}
	else if (!JibSailGoingUp && JibSailUp > 0.f)
	{
		JibSailUp -= DeltaTime;
		if (JibSailUp < 0.f) JibSailUp = 0.f;
		//pass on to dynaimc mats
		JibSailMat->SetScalarParameterValue("SailUp", JibSailUp);
		JibRopeMat->SetScalarParameterValue("Fade", JibSailUp);
	}
}

// Handle foam particle systems
void ABoat::HandleFoam(float DeltaTime)
{
	//no foam when flying
	if (FloatComp->GetIsInAir()) return; 

	//decals

	//calc forward speed to determine when next foam needs to spawn
	float NewMovement = FloatComp->GetVelocity() | GetActorForwardVector();

	FoamIntervalTimer += (NewMovement+FoamPassiveAdd)*DeltaTime;

	if (FoamIntervalTimer > FoamInterval)
	{
		FoamIntervalTimer -= FoamInterval;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		FVector FoamSpawnPoint = GetActorLocation();
		//spawn in front by velocity so that when foam fades in it appears correctly
		FoamSpawnPoint += GetActorForwardVector()*FoamOffset*FloatComp->GetVelocity().Size();
		UE_LOG(LogTemp, Log, TEXT("Velocity %f"), FloatComp->GetVelocity().Size());

		//always spawn 5cm above water surface
		FoamSpawnPoint.Z = 5.f;

		//spawn and set parameters based on boat forward speed
		AFoamDecal* NewFoam = GetWorld()->SpawnActor<AFoamDecal>(FoamSpawnPoint, FRotator::ZeroRotator, SpawnParams);
		NewFoam->Alpha = FMath::Clamp((NewMovement + FoamPassiveAdd) / 100.f, 0.f, 1.f);
		NewFoam->GrowRate = (NewMovement + FoamPassiveAdd) / 1000.f;
		NewFoam->RotateRate = FMath::RandRange(-20.f, 20.f);

	}

	//particles

	//amount of foam spray based on boat drag
	SprayTimer += FloatComp->GetCurrentDrag() * DeltaTime * SprayAmount;

	int32 SprayCount = FMath::FloorToInt(SprayTimer);
	SprayTimer -= SprayCount;

	if (SprayCount > 0)
	{
		for (int32 i = 0; i < SprayCount; i++)
		{
			//create at random position along foam spray spline
			float NewFoamPosition = FMath::RandRange(0.f, FoamSpline->GetSplineLength());
			FVector FoamPosition = FoamSpline->GetLocationAtDistanceAlongSpline(NewFoamPosition, ESplineCoordinateSpace::World);

			//velocity is at normal to the spline
			FVector FoamVelocity = FRotator(0.f, -90.f, 0.f).RotateVector(FoamSpline->GetTangentAtDistanceAlongSpline(NewFoamPosition, ESplineCoordinateSpace::World)).GetSafeNormal();
			FoamVelocity.Z = 1.f;
			FoamVelocity.Normalize();
			FoamVelocity *= FMath::RandRange(SprayMinSpeed, SprayMaxSpeed) * (FloatComp->GetCurrentDrag() * SprayAmount);

			//spawn
			FoamSpray->GenerateParticleEvent(FName("SpawnSpray"), 0.f, FoamPosition, FVector::ZeroVector, FoamVelocity);
		}
	}
}

// Handle camera positioning
void ABoat::HandleCamera(float DeltaTime)
{
	//trace from camera arm origin to end of camera arm

	FRotator CameraRot = GetController()->GetControlRotation();
	FVector CameraPos = GetActorLocation() + GetActorRotation().RotateVector(CameraOffset);
	FVector CameraArm = CameraRot.Vector()*-CameraDistance;
	
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
	TraceParams.bTraceComplex = false;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.AddIgnoredActor(this);

	FCollisionObjectQueryParams ObjectTraceParams = FCollisionObjectQueryParams();
	ObjectTraceParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	ObjectTraceParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	ObjectTraceParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);

	FHitResult TraceHit;
	if (GetWorld()->LineTraceSingleByObjectType(TraceHit, CameraPos, CameraPos + CameraArm, ObjectTraceParams, TraceParams))
	{
		//if trace hit, place camera at hit location
		CameraPos = TraceHit.Location;
	}
	else
	{
		//place camera at end of camera arm
		CameraPos += CameraArm;
	}

	//tilt camera
	CameraRot.Pitch += CameraTilt;
	
	//get game wave manager
	if (!WaveManager)
	{
		AGameMode* GameMode = UGameplayStatics::GetGameMode(this);
		if (GameMode)
		{
			ACustomMeshTestGameMode* CustomGameMode = Cast<ACustomMeshTestGameMode>(GameMode);
			if (CustomGameMode)
			{
				WaveManager = CustomGameMode->GetWaveManager();
			}
		}
	}

	//ensure camera does not go below waves
	if (WaveManager)
	{
		FVector WaveDisplacement = FVector::ZeroVector;
		FVector WaveNormal = FVector::ZeroVector;

		//get wave height at camera position
		WaveManager->GetWaveDisplacementNormal(FVector2D(CameraPos.X, CameraPos.Y), GetWorld()->GetTimeSeconds(), WaveDisplacement, WaveNormal);

		if (WaveDisplacement.Z > CameraPos.Z)
		{
			//set camera height to wave height
			CameraPos.Z = WaveDisplacement.Z;
		}
	}

	//actually move and rotate camera
	CameraComp->SetWorldLocationAndRotation(CameraPos, CameraRot);
}

//Handle boat motion in the ocean
void ABoat::HandleMovement(float DeltaTime)
{
	//Sail power
	FVector Wind = FVector::ZeroVector;

	//get game wind vector
	AGameMode* GameMode = UGameplayStatics::GetGameMode(this);
	if (GameMode)
	{
		ACustomMeshTestGameMode* CustomGameMode = Cast<ACustomMeshTestGameMode>(GameMode);
		if (CustomGameMode)
		{
			Wind = CustomGameMode->Wind;
		}
	}
	//find main sail normal in world space
	FVector WorldSailVector = (GetActorRotation() + FRotator(0.f, SailPosition, 0.f)).Vector();

	//find main power of wind caught in the sail
	float WindPower = (Wind | WorldSailVector)*SailUp;

	//find how much power main sail transfers to the boat
	float SailDrive = (((WindPower > 0) ? SailPower : SailBackPower)*WindPower*WorldSailVector) | GetActorForwardVector();

	//find jib sail normal in world space
	FVector WorldJibSailVector = (GetActorRotation() + FRotator(0.f, JibSailPosition, 0.f)).Vector();

	//find jib power of wind caught in the sail
	float JibWindPower = (Wind | WorldJibSailVector)*JibSailUp;

	//find how much power jib sail transfers to the boat
	float JibSailDrive = (((JibWindPower > 0) ? JibSailPower : JibSailBackPower)*JibWindPower*WorldJibSailVector) | GetActorForwardVector();

	//calculate total forward power provided by sails
	Power = SailPowerFunction(SailDrive) + SailPowerFunction(JibSailDrive);

	//oar power
	if (OarsOut)
	{
		//don't use oar power if sail power is more
		if ((OarPower > 0 && Power < OarPower) || (OarPower < 0 && Power > OarPower))
		{
			//add oar power
			Power += OarPower;

			//don't provide more power than oars are capable of on their own
			if ((OarPower > 0 && Power > OarPower) || (OarPower < 0 && Power < OarPower)) Power = OarPower;
		}

		//oar resist (if oars are out but sails are overcoming them, oars create drag)
		Power = OarPower + (OarResist * (Power - OarPower));
	}

	//pass on wind powers to anim
	if (BoatAnim) 
	{
		BoatAnim->SetWind(Wind.Rotation().Yaw - GetActorRotation().Yaw, WindPower);
		BoatAnim->SetJibWind(Wind.Rotation().Yaw - GetActorRotation().Yaw, JibWindPower);
	}
	UE_LOG(LogTemp, Log, TEXT("Main Sail Power = %f, Jib Power = %f, Wind Direction = %f, Total Power = %f"), WindPower, JibWindPower, Wind.Rotation().Yaw - GetActorRotation().Yaw, Power);

	//velocity

	FVector ForwardVectorOnOcean = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);

	//add total power to float physics velocity
	FloatComp->SetVelocity(FloatComp->GetVelocity() + ForwardVectorOnOcean * Power * DeltaTime);

	//add rudder rotation rate to float physics
	FloatComp->SetYawVelocity(FMath::FInterpConstantTo(FloatComp->GetYawVelocity(), RudderPosition * SteeringSpeed * (FloatComp->GetVelocity() | GetActorForwardVector()), DeltaTime, 180));



	//UE_LOG(LogTemp, Log, TEXT("WindPower = %f"), WindPower);

	//UE_LOG(LogTemp, Log, TEXT("Power = %f"), Power);
}
