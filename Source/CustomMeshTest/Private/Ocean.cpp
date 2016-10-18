// Visual only - procedural triagular shaped ocean mesh that moves with the waves and fills the player's field of view cone, giving the appearence of an endless ocean

#include "CustomMeshTest.h"
#include "ProceduralMeshComponent.h"
#include "WaveManager.h"
#include "CustomMeshTestGameMode.h"
#include "Ocean.h"



// Sets default values
AOcean::AOcean()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, false);

	//root component

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	//ocean procedural mesh

	OceanMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	OceanMesh->AttachToComponent(RootComponent, AttachRules);
	OceanMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OceanMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	OceanMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	//find ocean material

	static ConstructorHelpers::FObjectFinder<UMaterial> OceanMatClassFinder(TEXT("/Game/Ocean/Materials/TestOceanMat"));
	WaveMaterial = OceanMatClassFinder.Object;

	//find grid expansion curve

	static ConstructorHelpers::FObjectFinder<UCurveFloat> ExpansionCurveClassFinder(TEXT("/Game/Ocean/TestExpansionCurve"));
	GridExpansionCurve = ExpansionCurveClassFinder.Object;

	//default grid params

	GridCellSize = 100.f;
	GridStage = 90;
	GridYScale = 1.7f;
	GridXOffset = -1000.f;
	GridBuilt = false;
	GridUVSize = 1000;
}

// Called when the game starts or when spawned
void AOcean::BeginPlay()
{
	Super::BeginPlay();

	CreateGridVerts();
}

// Called every frame
void AOcean::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (GridBuilt && OceanMesh)
	{
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
			//declare mesh arrays
			TArray<FVector> Verts;
			TArray<FVector> Normals;
			TArray<FProcMeshTangent> Tangents;
			TArray<FVector2D> UV0;
			TArray<FColor> VertCols;

			//for every vert
			for (int32 i = 0; i < GridVerts.Num(); i++)
			{
				//calculate absolute world location of the vert based on grid
				FVector VertAbsolute = GetActorLocation() + GetActorRotation().RotateVector(GridVerts[i]);

				//find the displacement and normal of the wave at this location at the current game time
				FVector WaveDisplacement;
				FVector WaveNormal;
				WaveManager->GetWaveDisplacementNormal(FVector2D(VertAbsolute.X, VertAbsolute.Y), GetWorld()->GetTimeSeconds(), WaveDisplacement, WaveNormal);

				//calculate the relative location and normal of the vert to the ocean actor
				FVector VertAbsoluteDisplacement = VertAbsolute + WaveDisplacement;
				FVector VertRelativeDisplacement = GetActorRotation().UnrotateVector(VertAbsoluteDisplacement - GetActorLocation());
				FVector VertRelativeNormal = GetActorRotation().UnrotateVector(WaveNormal);

				//add vert data to be used by procedurally generated ocean mesh
				Verts.Add(VertRelativeDisplacement);
				Normals.Add(VertRelativeNormal);
				Tangents.Add(FProcMeshTangent());
			}
			//update procedurally generated ocean mesh
			OceanMesh->UpdateMeshSection(0, Verts, Normals, UV0, VertCols, Tangents);
		}
	}
}

// Create a triangular grid of vertices that will form the ocean
void AOcean::CreateGridVerts()
{
	//declare grid mesh arrays
	TArray<FVector> Verts;
	TArray<int32> Tris;

	//grid is created in stages - stage 1 is initial triangle, stage 2 is 3 more off that one, stage 3 is 5 more etc. to form a large triangular mesh
	for (int32 i = 0; i < GridStage; i++)
	{
		//first vert in line
		int32 StageInitialVert = Verts.Add(FVector((FMath::Sqrt(3.f)*GridCellSize)/2.f, GridCellSize/2.f, 0.f) * i);

		//each stage creates one more vert than the last, so stage number corresponds to the number of verts produced in that stage
		for (int32 j = 0; j < i; j++)
		{
			//next vert on line
			Verts.Add(Verts[StageInitialVert] - FVector(0.f, (j + 1)*GridCellSize, 0.f));

			//tri connecting that vert to the grid

			Tris.Add(StageInitialVert + j);
			Tris.Add(StageInitialVert + j + 1);
			Tris.Add(StageInitialVert - i + j);

			//tri filling the gap between new tri and the grid

			//no gap on last vert in line
			if (j < i - 1) 
			{
				Tris.Add(StageInitialVert - i + j);
				Tris.Add(StageInitialVert + j + 1);
				Tris.Add(StageInitialVert - i + j + 1);
			}
		}
	}

	//declare other mesh arrays
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FColor> VertCols;
	TArray<FProcMeshTangent> Tangents;

	//scale/offset the verts and add them to the final mesh grid

	GridVerts.Empty();
	for (int32 i = 0; i < Verts.Num(); i++)
	{
		FVector ExpandedVert = Verts[i];
		if (GridExpansionCurve)
		{
			float ExpansionFactor = GridExpansionCurve->GetFloatValue(Verts[i].X); 
			ExpandedVert *= ExpansionFactor;
		}
		ExpandedVert.Y *= GridYScale;
		ExpandedVert.X += GridXOffset;

		GridVerts.Add(ExpandedVert);

		//calculate UV coordinates
		UV0.Add(FVector2D(ExpandedVert.X, ExpandedVert.Y) / GridUVSize);
	}	

	if (OceanMesh)
	{
		//create procedural mesh from the grid
		OceanMesh->CreateMeshSection(0, GridVerts, Tris, Normals, UV0, VertCols, Tangents, false);
		if (WaveMaterial)
		{
			OceanMesh->SetMaterial(0, WaveMaterial);
		}
		GridBuilt = true;
		UE_LOG(LogTemp, Log, TEXT("Grid Complete, %d tris."), Tris.Num());
	}
}