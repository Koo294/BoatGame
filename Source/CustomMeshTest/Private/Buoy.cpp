// Bouy which floats upon the ocean and gives information such as wind direction and compass

#include "CustomMeshTest.h"
#include "FloatComponent.h"
#include "FoamDecal.h"
#include "CustomMeshTestGameMode.h"
#include "BuoyAnimInstance.h"
#include "Buoy.h"


// Sets default values
ABuoy::ABuoy()
{
	PrimaryActorTick.bCanEverTick = true;

	//bouy mesh and anims
	BuoyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BuoyMesh"));
	RootComponent = BuoyMesh;

	//find mesh and animation
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BuoyMeshClassFinder(TEXT("/Game/buoy"));
	static ConstructorHelpers::FClassFinder<UAnimInstance> BuoyAnimClassFinder(TEXT("/Game/BuoyAnimBP"));
	BuoyMesh->SetSkeletalMesh(BuoyMeshClassFinder.Object);
	BuoyMesh->SetAnimInstanceClass(BuoyAnimClassFinder.Class);

	//make sure it blocks the camera
	BuoyMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BuoyMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BuoyMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	//float physics
	FloatComp = CreateDefaultSubobject<UFloatComponent>(TEXT("FloatComp"));

	//part params
	VaneResist = 0.4f;

	//foam params
	FoamInterval = 2.f;
	FoamSpawnDistance = 3000.f;
}

// Called when the game starts or when spawned
void ABuoy::BeginPlay()
{
	Super::BeginPlay();

	//set anim for code access
	BuoyAnim = Cast<UBuoyAnimInstance>(BuoyMesh->GetAnimInstance());
	
}

// Called every frame
void ABuoy::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

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

	//lerp toward rotations for compass and wind vane
	VaneRot = FMath::Lerp(Wind.Rotation().Yaw - GetActorRotation().Yaw, VaneRot, FMath::Pow(VaneResist, DeltaTime));
	CompassRot = FMath::Lerp(-GetActorRotation().Yaw, CompassRot, FMath::Pow(VaneResist, DeltaTime));

	if (BuoyAnim)
	{
		//pass on to anim
		BuoyAnim->SetWind(VaneRot);
		BuoyAnim->SetNorth(CompassRot);
	}	
	
	//foam decals
	FoamIntervalTimer += DeltaTime;

	//don't spawn foam if foam is too far away from player to prevent unessecary performance loss
	if ((UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation() - GetActorLocation()).Size() < FoamSpawnDistance && FoamIntervalTimer > FoamInterval)
	{
		FoamIntervalTimer -= FoamInterval;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		//spawm foam decal
		AFoamDecal* NewFoam = GetWorld()->SpawnActor<AFoamDecal>(FVector(GetActorLocation().X, GetActorLocation().Y, 5.f), FRotator::ZeroRotator, SpawnParams);
		NewFoam->RotateRate = FMath::RandRange(-20.f, 20.f);

	}
}

