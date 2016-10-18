// Visual only - Foam decal created by boat or other object floating on the ocean's surface to represent small waves created (e.g bow waves)

#include "CustomMeshTest.h"
#include "ProceduralMeshComponent.h"
#include "FoamDecal.h"


// Sets default values
AFoamDecal::AFoamDecal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//find material

	static ConstructorHelpers::FObjectFinder<UMaterial> FoamMatClassFinder(TEXT("/Game/Boat/Effects/Materials/FoamDecalMat"));
	WaveMaterial = FoamMatClassFinder.Object;

	//find alpha curve

	static ConstructorHelpers::FObjectFinder<UCurveFloat> AlphaLifeCurveClassFinder(TEXT("/Game/Boat/Effects/FoamAlphaLifeCurve"));
	AlphaLifeCurve = AlphaLifeCurveClassFinder.Object;

	//lifetime params

	RotateRate = 20.f;
	Alpha = 1.f;
	GrowRate = 0.2f;
	CurrentGrowth = 0.f;
}

// Called when the game starts or when spawned
void AFoamDecal::BeginPlay()
{
	Super::BeginPlay();

	//create dynamic mats

	DynamicFoamMat = UMaterialInstanceDynamic::Create(WaveMaterial, OceanMesh);
	OceanMesh->SetMaterial(0, DynamicFoamMat);
	
	//random rotation on spawn

	SetActorRotation(GetActorRotation() + FRotator(0.f, FMath::RandRange(-180.f, 180.f), 0.f));
}

// Called every frame
void AFoamDecal::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	//increase lifetime

	Life += DeltaTime;

	//destroy at end of life

	float LifeStart, LifeEnd = 0;

	AlphaLifeCurve->GetTimeRange(LifeStart, LifeEnd);
	if (Life > LifeEnd - LifeStart)
	{
		Destroy();
	}

	//handle lifetime changes

	SetActorRotation(GetActorRotation() + FRotator(0.f, RotateRate*DeltaTime, 0.f));

	CurrentGrowth += GrowRate*DeltaTime;

	//update dynamic mats

	DynamicFoamMat->SetScalarParameterValue(FName("Alpha"), AlphaLifeCurve->GetFloatValue(Life)*Alpha);

	DynamicFoamMat->SetScalarParameterValue(FName("Scale"), CurrentGrowth);
}

