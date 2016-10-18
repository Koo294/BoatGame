// Visual only - decal that appears on the surface of the ocean and moves with the waves

#include "CustomMeshTest.h"
#include "ProceduralMeshComponent.h"
#include "WaveManager.h"
#include "CustomMeshTestGameMode.h"
#include "OceanDecal.h"


// Sets default values
AOceanDecal::AOceanDecal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, false);

	//root component

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	//decal procedural mesh

	OceanMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	OceanMesh->AttachToComponent(RootComponent, AttachRules);

	//find ocean material

	static ConstructorHelpers::FObjectFinder<UMaterial> OceanMatClassFinder(TEXT("/Game/Ocean/Materials/TestOceanMat"));
	WaveMaterial = OceanMatClassFinder.Object;

	//default grid params

	GridCellSize = 100.f;
	GridStage = 5;
	GridBuilt = false;

}

// Called when the game starts or when spawned
void AOceanDecal::BeginPlay()
{
	Super::BeginPlay();

	CreateGridVerts();
	
}

// Called every frame
void AOceanDecal::Tick( float DeltaTime )
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

				//calculate the relative location and normal of the vert to the decal actor
				FVector VertAbsoluteDisplacement = VertAbsolute + WaveDisplacement;
				FVector VertRelativeDisplacement = GetActorRotation().UnrotateVector(VertAbsoluteDisplacement - GetActorLocation());
				FVector VertRelativeNormal = GetActorRotation().UnrotateVector(WaveNormal);

				//add vert data to be used by procedurally generated decal mesh
				Verts.Add(VertRelativeDisplacement);
				Normals.Add(VertRelativeNormal);
				Tangents.Add(FProcMeshTangent());
			}
			//update procedurally generated decal mesh
			OceanMesh->UpdateMeshSection(0, Verts, Normals, UV0, VertCols, Tangents);
		}
	}

}

// Create a hexagonal grid of vertices that will form the ocean decal
void AOceanDecal::CreateGridVerts()
{
	//declare grid mesh arrays
	TArray<FGridVert> Verts;
	TArray<int32> Tris;
	TArray<FGridEdge> Edges;

	//initial triangle
	Verts.Add(FGridVert(FVector(0, -FMath::Sqrt(0.75)*GridCellSize / 2.f, 0), 0, true));
	Verts.Add(FGridVert(FVector(-0.5*GridCellSize, FMath::Sqrt(0.75)*GridCellSize / 2.f, 0), 0, true));
	Verts.Add(FGridVert(FVector(0.5*GridCellSize, FMath::Sqrt(0.75)*GridCellSize / 2.f, 0), 0, true));

	Edges.Add(FGridEdge(0, 1));
	Edges.Add(FGridEdge(1, 2));
	Edges.Add(FGridEdge(2, 0));

	Tris.Add(0);
	Tris.Add(1);
	Tris.Add(2);

	//grid is created in stages - each stage covers the outside edge of the grid in a new layer of tris
	for (int32 i = 0; i < GridStage; i++)
	{
		//keep track of the edges created in this stage - outside edges will become the base to build off next stage
		TArray<FGridEdge> NewEdges;

		//on every outside edge
		for (int32 j = 0; j < Edges.Num(); j++)
		{
			//if both verts in this edge are the same stage, form an OnPoint tri
			if (Verts[Edges[j].V1].Stage == Verts[Edges[j].V2].Stage)
			{
				//create OnPoint vert - creating a triangle pointing away from the edge (located on flat hexagon corners)
				FVector2D NewVertPos = TriangulateEdge(Verts[Edges[j].V1].Vert, Verts[Edges[j].V2].Vert);

				int32 NewVertIndex = Verts.Num();
				Verts.Add(FGridVert(FVector(NewVertPos.X, NewVertPos.Y, 0), i + 1, true));

				NewEdges.Add(FGridEdge(Edges[j].V1, NewVertIndex));
				NewEdges.Add(FGridEdge(NewVertIndex, Edges[j].V2));

				Tris.Add(Edges[j].V1);
				Tris.Add(NewVertIndex);
				Tris.Add(Edges[j].V2);
			}
			//if first vert is later stage than second vert in this edge, then a Cluster is started
			else if (Verts[Edges[j].V1].Stage > Verts[Edges[j].V2].Stage)
			{
				//Start of a tri Cluster - group of tris along an edge that are completed by an ClusterEnd tri

				//check if true cluster (true clusters only form on top of OnPoint tris)
				if (Verts[Edges[j].V2].OnPoint)
				{
					//start the true cluster (group of 3 triangles, with 2 new verts - located on top of a OnPoint tri, so also at hexagon corners)
					FVector2D NewVertPos = TriangulateEdge(Verts[Edges[j].V1].Vert, Verts[Edges[j].V2].Vert);

					int32 NewVertIndex = Verts.Num();
					Verts.Add(FGridVert(FVector(NewVertPos.X, NewVertPos.Y, 0), i + 1, false));

					NewEdges.Add(FGridEdge(Edges[j].V1, NewVertIndex));

					Tris.Add(Edges[j].V1);
					Tris.Add(NewVertIndex);
					Tris.Add(Edges[j].V2);

					//Shared Vert - does not exist yet but will be created by the ClusterEnd
					int32 SharedVertIndex = NewVertIndex + 1;
					if (j == Edges.Num() - 1)
					{
						SharedVertIndex = NewEdges[0].V1;
					}

					NewEdges.Add(FGridEdge(NewVertIndex, SharedVertIndex));

					Tris.Add(Edges[j].V2);
					Tris.Add(NewVertIndex);
					Tris.Add(SharedVertIndex);
				}
				else
				{
					//start the false cluster (group of 2 triangles, with 1 new vert - located along the edges of the hexagon grid)

					//Shared Vert - does not exist yet but will be created by the ClusterEnd
					int32 SharedVertIndex = Verts.Num();
					if (j == Edges.Num() - 1)
					{
						SharedVertIndex = NewEdges[0].V1;
					}

					NewEdges.Add(FGridEdge(Edges[j].V1, SharedVertIndex));

					Tris.Add(Edges[j].V1);
					Tris.Add(SharedVertIndex);
					Tris.Add(Edges[j].V2);
				}
			}
			//if first vert is earlier stage than second vert in this edge, then a Cluster can be completed here
			else
			{
				//ClusterEnd - this completes the partially formed true or false cluster
				FVector2D NewVertPos = TriangulateEdge(Verts[Edges[j].V1].Vert, Verts[Edges[j].V2].Vert);

				int32 NewVertIndex = Verts.Num();
				Verts.Add(FGridVert(FVector(NewVertPos.X, NewVertPos.Y, 0), i + 1, false));

				NewEdges.Add(FGridEdge(NewVertIndex, Edges[j].V2));

				Tris.Add(Edges[j].V1);
				Tris.Add(NewVertIndex);
				Tris.Add(Edges[j].V2);
			}
		}
		//set the edges to be built upon next stage to the edges formed this stage
		Edges = NewEdges;
		NewEdges.Empty();

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
		FVector ExpandedVert = Verts[i].Vert;
		GridVerts.Add(ExpandedVert);

		//calculate UV coordinates
		UV0.Add(FVector2D(ExpandedVert.X, ExpandedVert.Y) / (GridCellSize*GridStage) + FVector2D(0.5f, 0.5f));
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

// Find the point that will form an equilatiral triangle with 2 given points
FVector2D AOceanDecal::TriangulateEdge(FVector V1, FVector V2)
{
	//calculate vector between 2 given points
	FVector2D EdgeVector = FVector2D(V2.X - V1.X, V2.Y - V1.Y);
	EdgeVector.Normalize();
	EdgeVector *= GridCellSize;

	//calculate dividing normal of the equilatiral triangle
	FVector2D EdgeNormal = FVector2D(-EdgeVector.Y, EdgeVector.X);
	EdgeNormal.Normalize();
	EdgeNormal *= FMath::Sqrt(3 * EdgeVector.SizeSquared() / 4);

	//calculate relative position of the equilatiral triangle top point from the first point
	FVector2D RelativeOffset = EdgeVector / 2 + (EdgeNormal);

	return FVector2D(V1.X + RelativeOffset.X, V1.Y + RelativeOffset.Y);
}