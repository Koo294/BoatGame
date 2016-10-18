// Visual only - procedural triagular shaped ocean mesh that moves with the waves and fills the player's field of view cone, giving the appearence of an endless ocean

#pragma once

#include "GameFramework/Actor.h"
#include "Ocean.generated.h"

UCLASS()
class CUSTOMMESHTEST_API AOcean : public AActor
{
	GENERATED_BODY()

private:

	//ocean grid building

	TArray<FVector> GridVerts;

	// Create a triangular grid of vertices that will form the ocean
	void CreateGridVerts();

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
	AOcean();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//ocean grid parameters

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshGrid)
	int32 GridStage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshGrid)
	UCurveFloat*  GridExpansionCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshGrid)
	float GridYScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshGrid)
	float  GridCellSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshGrid)
	float GridXOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshGrid)
	float GridUVSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterial* WaveMaterial;
};
