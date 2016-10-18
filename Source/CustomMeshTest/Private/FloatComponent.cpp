// Component simulating the physics of an object in the ocean, floating on the waves

#include "CustomMeshTest.h"
#include "CustomMeshTestGameMode.h"
#include "WaveManager.h"
#include "FloatComponent.h"


// Sets default values for this component's properties
UFloatComponent::UFloatComponent()
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	Drag = 0.4f;
	SideDrag = 0.f;
	Gravity = 981.f;
	Bouyancy = 20.f;
	BouyancyAngular = 250.f;
	BouyancyDrag = 0.05f;
	BouyancyAngularResist = 0.05f;
	IsInAir = false;
	Slide = 2.f;
}


// Called when the game starts
void UFloatComponent::BeginPlay()
{
	Super::BeginPlay();

	//set initial ocean position to owner location
	PositionOnOcean = GetOwner()->GetActorLocation();
}


// Called every frame
void UFloatComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

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
	if (WaveManager)
	{

		FVector ForwardVectorOnOcean = FRotationMatrix(GetOwner()->GetActorRotation()).GetScaledAxis(EAxis::X);


		if (!IsInAir) //only drag in water
		{

			FVector VelocityNormal = Velocity.GetSafeNormal();

			//calculate drag based on velocity and oreintation - moving = more drag, moving sideways = even more drag (SideDrag)
			float CurrentDrag = Drag * FMath::Pow(FMath::Abs((1.f - VelocityNormal.Size()) - (ForwardVectorOnOcean | VelocityNormal)), SideDrag);

			//Velocity due to sliding on waves
			FVector DropVector = FVector::VectorPlaneProject(FRotationMatrix(GetOwner()->GetActorRotation()).GetScaledAxis(EAxis::Z), FVector(0.f, 0.f, -1.f));
			Velocity += DropVector * Slide;

			//Drag
			Velocity.X *= FMath::Pow(CurrentDrag, DeltaTime);
			Velocity.Y *= FMath::Pow(CurrentDrag, DeltaTime);
			//Not the Z as this is used for bouyancy which has its own drag

			//estimate moving average for spray
			CurrentCombinedDrag += 0.05 * ((Velocity - (Velocity * CurrentDrag)).Size() - CurrentCombinedDrag);
		}

		//add velocity to ocean position
		PositionOnOcean.X += Velocity.X * DeltaTime;
		PositionOnOcean.Y += Velocity.Y * DeltaTime;

		//Gravity and Bouyancy

		FVector WaveDisplacement = FVector::ZeroVector;
		FVector WaveNormal = FVector::ZeroVector;

		//find wave displacement and normal at owner's location
		WaveManager->GetWaveDisplacementNormal(FVector2D(GetOwner()->GetActorLocation().X, GetOwner()->GetActorLocation().Y), GetWorld()->GetTimeSeconds(), WaveDisplacement, WaveNormal);

		//where the boat should be on the water
		FVector NextPosition = FVector(PositionOnOcean.X + WaveDisplacement.X, PositionOnOcean.Y + WaveDisplacement.Y, GetOwner()->GetActorLocation().Z);

		//if above where it should be
		if (NextPosition.Z > (PositionOnOcean + WaveDisplacement).Z)
		{
			//apply gravity
			Velocity.Z -= Gravity*DeltaTime;
			//not in air if gravity has brought it back below again
			IsInAir = (NextPosition.Z + (Velocity.Z)*DeltaTime > PositionOnOcean.Z + WaveDisplacement.Z); 
		}
		else
		{
			IsInAir = false;
			
			//apply bouyancy - more bouyancy the more underwater it is
			Velocity.Z += Bouyancy*DeltaTime*((PositionOnOcean + WaveDisplacement).Z - NextPosition.Z);
			//bouyancy drag
			Velocity.Z *= FMath::Pow(BouyancyDrag, DeltaTime);
		}

		//apply gravity/bouyancy to position
		NextPosition.Z += Velocity.Z * DeltaTime;

		//actually set new location
		GetOwner()->SetActorLocation(NextPosition);

		//Rotation

		if (IsInAir)
		{
			//just keep spinning
			GetOwner()->SetActorRotation(GetOwner()->GetActorRotation() + AVelocity*DeltaTime); 
		}
		else
		{
			//perfectly placed onto ocean surface and normal
			FRotator IdealOceanRot = FRotationMatrix::MakeFromXY(FVector::VectorPlaneProject(ForwardVectorOnOcean, WaveNormal), FVector::VectorPlaneProject(FRotationMatrix(GetOwner()->GetActorRotation()).GetScaledAxis(EAxis::Y), WaveNormal)).Rotator();

			FRotator Delta = (IdealOceanRot - GetOwner()->GetActorRotation());

			//"Normalising" the rotation so it's only 1 degree in any direction max

			float DeltaAngleSize = FMath::Abs(Delta.Yaw) + FMath::Abs(Delta.Pitch) + FMath::Abs(Delta.Roll);//FMath::RadiansToDegrees(FMath::Acos(FVector::ForwardVector | DeltaVector));

			if (DeltaAngleSize > 1) //no dividing by zero
			{
				Delta *= 1 / DeltaAngleSize;
			}

			AVelocity.Pitch += Delta.Pitch * DeltaTime * BouyancyAngular;
			AVelocity.Roll += Delta.Roll * DeltaTime * BouyancyAngular;
			//Yaw is using rudder (if boat), not governed by the ocean normal - is set by the boat
			AVelocity *= FMath::Pow(BouyancyAngularResist, DeltaTime);


			//for some reason AddActorWorldRotation causes problems
			FRotator NewRotation = GetOwner()->GetActorRotation() + AVelocity*DeltaTime;

			GetOwner()->SetActorRotation(NewRotation);
		}
	}
}

// Input angular yaw velocity
void UFloatComponent::SetYawVelocity(float Val)
{
	AVelocity.Yaw = Val;
}

float UFloatComponent::GetYawVelocity()
{
	return AVelocity.Yaw;
}

// Estimate moving average of drag affecting the float
float UFloatComponent::GetCurrentDrag()
{
	return CurrentCombinedDrag;
}

// Input velocity
void UFloatComponent::SetVelocity(FVector Val)
{
	Velocity = Val;
}

FVector UFloatComponent::GetVelocity()
{
	return Velocity;
}

bool UFloatComponent::GetIsInAir()
{
	return IsInAir;
}

