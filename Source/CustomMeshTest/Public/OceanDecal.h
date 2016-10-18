// Visual only - decal that appears on the surface of the ocean and moves with the waves

#pragma once

#include "GameFramework/Actor.h"
#include "OceanDecal.generated.h"

//represents an edge between 2 points on the grid
struct FGridEdge
{
	int32 V1;
	int32 V2;

	FGridEdge() { }

	FGridEdge(int32 V1Set, int32 V2Set)
	{
		V1 = V1Set;
		V2 = V2Set;
	}
};

//represents a vert on the grid with additional grid information
struct FGridVert
{
	FVector Vert;

	//grid stage this vert was created
	int32 Stage;

	//whether this vert was on the point of a single triangle or a flat between 2 triangles in its stage
	bool OnPoint;

	FGridVert() { }

	FGridVert(FVector VertSet, int32 StageSet, bool OnPointSet)
	{
		Vert = VertSet;
		Stage = StageSet;
		OnPoint = OnPointSet;
	}
};

UCLASS()
class CUSTOMMESHTEST_API AOceanDecal : public AActor
{
	GENERATED_BODY()

	//ocean grid building

	TArray<FVector> GridVerts;

	// Create a hexagonal grid of vertices that will form the ocean decal
	void CreateGridVerts();

	// Find the point that will form an equilatiral triangle with 2 given points
	FVector2D TriangulateEdge(FVector V1, FVector V2);

	bool GridBuilt;

	//game wave manager
	class UWaveManager* WaveManager;

protected:

	//actor components

	UPROPERTY(EditAnywhere, Category = Components)
	USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere, Category = Components)
	class UProceduralMeshComponent* OceanMesh;

public:	
	// Sets default values for this actor's properties
	AOceanDecal();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//ocean grid parameters

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshGrid)
	int32 GridStage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshGrid)
	float  GridCellSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterial* WaveMaterial;
	
};
